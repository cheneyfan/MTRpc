#include "rpc_channel.h"
#include "rpc_channel_impl.h"
#include "mio/mio_error_code.h"
#include "mio/mio_connect_stream.h"
#include "rpc_controller_impl.h"
#include "log/log.h"

namespace mtrpc {

RpcChannelImpl::RpcChannelImpl(const RpcChannelOptions &options):
    _options(options),
    _stream(NULL),
    _poller(NULL),
    _group(NULL)
{

}

RpcChannelImpl::~RpcChannelImpl()
{
    if(_stream)
    {
        _stream->ReleaseRef();
    }
}

bool RpcChannelImpl::IsAddressValid()
{
    return true;
}


RpcController *RpcChannelImpl::GetController()
{
    RpcClientController* cntl = new RpcClientController();
    return cntl;
}

int RpcChannelImpl::Connect(const std::string& server_ip,int32_t server_port){

    _stream = new ConnectStream();

    _stream->group = _group;
    _stream->RequireRef();

    _stream->handerMessageRecived =
            NewPermanentExtClosure(this,&RpcChannelImpl::OnMessageRecived);

    _stream->handerMessageSended =
            NewPermanentExtClosure(this,&RpcChannelImpl::OnMessageSended);

    _stream->handerClose  =
            NewPermanentExtClosure(this,&RpcChannelImpl::OnClose);

    int ret = _stream->Connect(server_ip, server_port);

   if( ret == CLIENT_CONNECT_OK)
    {
        _stream->AddEventASync(_poller,true,false);
        return 0;
    }

   if(ret == CLIENT_CONNECT_IPROCESS)
   {
       _stream->AddEventASync(_poller,true,true);
       _stream->Wait();
   }

    if( _stream->_ConnectStatus == CLIENT_CONNECT_OK)
    {
        return 0;
    }

    // release
    _stream->ReleaseRef();
    _stream = NULL;
    return -1;
}




void RpcChannelImpl::OnClose(SocketStream *sream, Epoller *p){

    _stream->ReleaseRef();
    //TODO reset sream
}


void RpcChannelImpl::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                                ::google::protobuf::RpcController* controller,
                                const ::google::protobuf::Message* request,
                                ::google::protobuf::Message* response,
                                ::google::protobuf::Closure* done){

    RpcClientController * cntl = (RpcClientController * )controller;

    if(_stream == NULL
            || _stream->_ConnectStatus == CLIENT_CONNECT_FAIL)
    {
        cntl->SetStatus(CLIENT_CONNECT_FAIL);
        if(done)
            done->Run();
        return ;
    }

    cntl->_poller =_poller;
    cntl->_stream = _stream;
    cntl->_request = const_cast<Message*>(request);
    cntl->_response = const_cast<Message*>(response);
    cntl->_method = const_cast< ::google::protobuf::MethodDescriptor*>(method);
    cntl->_done = done;

    //invoid the call by event thread
    {
        WriteLock<MutexLock> lc(pendinglock);
        if(!pendingcall.empty()){
            pendingcall.push(cntl);
            return;
        }

        pendingcall.push(cntl);
        SendToServer(method, (RpcClientController*)controller, request);
    }

    //sync
    if(done == NULL)
        cntl->Wait();
}

int RpcChannelImpl::SendToServer(const ::google::protobuf::MethodDescriptor* method,
                                 RpcClientController* cntl,
                                 const ::google::protobuf::Message* request){

    HttpRequestHeader& reqheader = _stream->reqheader;
    reqheader.Reset();

    WriteBuffer& buf = _stream->writebuf;
    buf.Reset();

    reqheader.SetPath(method->full_name());
    reqheader.SetSeq(cntl->GetSeq());

    if(!buf.Reserve(MAX_HEADER_SIZE))
    {
        OnMessageError(_stream,_poller,SERVER_WRIEBUFFER_FULL);
        return -1;
    }

    WriteBuffer::Iterator packetStart = buf.writepos;
    WriteBuffer::Iterator bodyStart   = buf.AlignWritePos();

    if(!request->SerializeToZeroCopyStream(&buf))
    {
        //roll back
        buf.writepos = packetStart;
        OnMessageError(_stream,_poller,SERVER_RES_SERIA_FAILED);
        return -1;
    }

    uint32_t content_length  = buf.writepos - bodyStart;
    reqheader.SetContentLength(content_length);
    reqheader.SerializeHeader(packetStart);

    cntl->_req_pack_size = buf.writepos - packetStart;

    TRACE("send "<<cntl->_req_pack_size <<",header:"<<packetStart.get()->buffer);

    _stream->ModEventAsync(_poller,true,true);

    return 0;
}

void RpcChannelImpl::OnMessageSended(ConnectStream* stream,Epoller* p,uint32_t buffer_size){

     RpcClientController *  cntl = NULL;
    {
        WriteLock<MutexLock> lc(pendinglock);
        if(pendingcall.empty() )
        {
            stream->ModEventAsync(p,true,false);
            return;
        }

        cntl = pendingcall.front();
    }


    cntl ->_req_send_size  += buffer_size;

    if(cntl ->_req_send_size < cntl ->_req_pack_size)
    {
        return ;
    }
}

void RpcChannelImpl::OnMessageRecived(ConnectStream *sream, Epoller* p,uint32_t buffer_size){

    RpcClientController *  cntl = NULL;

    {
        WriteLock<MutexLock> wl(pendinglock);
        cntl = pendingcall.front();
        pendingcall.pop();
    }

    assert(cntl!=NULL);

    int ContentLength = sream->resheader.GetContentLength();

    if(ContentLength >0
            && !cntl->_response->ParseFromZeroCopyStream(&sream->readbuf))
    {
        OnMessageError(sream,p,SERVER_REQ_PARSER_FAILED);
        return;

    }

    //notify the call
    cntl->SetStatus(sream->resheader.status);

    if(cntl->_done)
        cntl->_done->Run();

    cntl = NULL;

    {
        WriteLock<MutexLock> wl(pendinglock);
        if(!pendingcall.empty())
        {
            cntl =  pendingcall.front();
        }
    }

    if(cntl == NULL)
        _stream->ModEventAsync(p,true,false);
    else
        SendToServer(cntl->_method, cntl, cntl->_request);
}


void RpcChannelImpl::OnMessageError(SocketStream* stream, Epoller* p, uint32_t error_code)
{
      RpcClientController *  cntl = NULL;
      {
          WriteLock<MutexLock> wl(pendinglock);
          // 1 remove all pending
          if(!pendingcall.empty())
          {
              cntl = pendingcall.front();
              pendingcall.pop();
          }
      }

      //notify client
      if(cntl){
          cntl->SetStatus(error_code);
      }

      {
          WriteLock<MutexLock> wl(pendinglock);
          // 1 remove all pending
          while(!pendingcall.empty())
          {
              cntl = pendingcall.front();
              pendingcall.pop();
              cntl->SetStatus(CLIENT_CANCEL_BY_ERROR);
          }
      }

      //2 send fail header
      stream->ModEventAsync(p, true, true);
      stream->_close_when_empty = true;
}



}
