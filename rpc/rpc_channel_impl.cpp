#include "rpc_channel.h"
#include "rpc_channel_impl.h"
#include "mio/mio_error_code.h"
#include "mio/mio_connect_stream.h"
#include "rpc_controller_impl.h"
#include "log/log.h"
#include "thread/workpool.h"
#include "google/protobuf/message.h"
#include "google/protobuf/descriptor.h"
#include "mio/mio_poller.h"
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
/*
    if(_stream)
    {
        if(_stream->_ConnectStatus == CLIENT_CONNECT_OK 
                && _stream->_isClosed == false)
        {
            _stream->OnClose(_poller);
        }

        //_stream->ReleaseRef();
        
        TRACE("~RpcChannelImpl"<<_stream->_fd<<" con status:"<<_stream->_ConnectStatus
                <<",isclose:"<<_stream->_isClosed
                <<",is_isConnected:"<<_stream->_isConnected<<",ref;"<<_stream->ref_count);

    }
    */


    TRACE("~RpcChannelImpl _stream:");
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

    if(_group->workerNum > 1)
         _stream->group = _group;

    _stream->RequireRef();

    _stream->handerMessageRecived =
            NewPermanentExtClosure(this,&RpcChannelImpl::OnMessageRecived);

    _stream->handerMessageSended =
            NewPermanentExtClosure(this,&RpcChannelImpl::OnMessageSended);

    _stream->handerClose  =
            NewPermanentExtClosure(this,&RpcChannelImpl::OnClose);

    _stream->handerWriteable =
            NewPermanentExtClosure(this,&RpcChannelImpl::OnWriteable);

    _stream->handerReadTimeOut =
            NewPermanentExtClosure(this,&RpcChannelImpl::OnRecvTimeout);

    _stream->handerWriteTimeOut =
            NewPermanentExtClosure(this,&RpcChannelImpl::OnSendTimeout);

    int ret = _stream->Connect(server_ip, server_port);


    if( ret == CLIENT_CONNECT_OK)
    {
        _stream->AddEventASync(_poller,true,false);
        return 0;
    }

    if(ret != CLIENT_CONNECT_IPROCESS)
    {
        //_stream->ReleaseRef();
        _stream = NULL;
        return -1;
    }

    // wait connect ok or failed
    _stream->AddEventASync(_poller, true, true);
    _stream->Wait();

    if( _stream->_ConnectStatus == CLIENT_CONNECT_OK)
    {
        TRACE("connect ok:"<<server_ip<<","<<server_port);
        return 0;
    }

    //_stream->ReleaseRef();
    //Will delete by closed
    return -1;
}


int RpcChannelImpl::Close()
{

    if(!_stream->_isClosed)
    {
        _stream->_close_when_empty = true;
        _stream->ModEventAsync(_poller, true, true);
    }

    //_stream->ReleaseRef();
}


void RpcChannelImpl::OnClose(SocketStream *sream, Epoller *p){

    //
    //TODO reset sream
    //delete this;
    TRACE("close:"<<sream->GetSockName()<<",send empty :"<<sendcall.empty()<<",wait size:"<<waitcall.size()<<",done size :"<<donecall.size());

    // clear the pending resource
    if(!sendcall.empty() || !waitcall.empty() || !donecall.empty())
        OnMessageError(sream, p, CLIENT_CANCEL_BY_CLOSE);

    sream->ReleaseRef();
    //release this
    delete this;
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
        sendcall.push(cntl);
        _stream->ModEventAsync(_poller, true, true);
    }

    //sync
    if(done == NULL)
        cntl->Wait();
}

void RpcChannelImpl::OnWriteable(SocketStream* stream,Epoller* p)
{
    if(sendcall.empty())
    {
        WARN(_stream->GetSockName()<< "not sendcall pending");
        //stream->ModEventAsync(p, true, false);
        return;
    }

    if(waitcall.empty()&& donecall.empty())
    {

        RpcClientController* cntl = NULL;
        sendcall.pop(cntl);

        SendToServer(cntl->_method, cntl, cntl->_request);
        // wait the read time out
        // _stream->SetWriteTimeOutAsync(_poller, _options.write_timeout_sec);
        waitcall.push(cntl);
    }
}

int RpcChannelImpl::SendToServer(const ::google::protobuf::MethodDescriptor* method,
                                 RpcClientController* cntl,
                                 const ::google::protobuf::Message* request){

    HttpRequestHeader& reqheader = _stream->reqheader;
    reqheader.Reset();

    WriteBuffer& buf = _stream->writebuf;
    buf.Reset();

    reqheader.SetPath("/"+method->full_name());

    uint64_t seq = cntl->GenerateSeq();
    cntl->_seq = seq;
    reqheader.SetSeq(seq);

    if(!buf.Reserve(MAX_HEADER_SIZE))
    {
        OnMessageError(_stream,_poller,SERVER_WRIEBUFFER_FULL);
        cntl->SetStatus(SERVER_WRIEBUFFER_FULL);
        return -1;
    }

    WriteBuffer::Iterator packetStart = buf.writepos;
    WriteBuffer::Iterator bodyStart   = buf.AlignWritePos();

    buf.BeginPacket();

    if(!request->SerializeToZeroCopyStream(&buf))
    {
        //roll back
        buf.writepos = packetStart;
        OnMessageError(_stream,_poller,SERVER_RES_SERIA_FAILED);
        cntl->SetStatus(SERVER_RES_SERIA_FAILED);
        return -1;
    }

    uint32_t content_length  = buf.writepos - bodyStart;
    reqheader.SetContentLength(content_length);

    reqheader.SerializeHeader(packetStart);
    //TRACE("packetStart:"<<packetStart.toString()<<",size:"<<packetStart.get()->size<<",length:"<<strlen(packetStart.get()->buffer+packetStart._pos));

    cntl->_req_pack_size = buf.writepos - packetStart;

    //TRACE(_stream->GetSockName()<<",send:"<<cntl->_req_pack_size);

    _stream->ModEventAsync(_poller, true, true);
    return 0;
}

void RpcChannelImpl::OnMessageSended(ConnectStream* stream,Epoller* p,int32_t buffer_size){

    while(!waitcall.empty() && buffer_size > 0)
    {
        RpcClientController * cntl = waitcall.front();
        cntl ->_req_send_size += buffer_size;

        if(cntl ->_req_send_size >= cntl ->_req_pack_size)
        {
            TRACE(stream->GetSockName()<<",has send:"<<cntl->_method->name()<<",seq:"<<cntl->_seq<<",pack_size:"<<cntl ->_req_pack_size<<",sended:"<<cntl ->_req_send_size<<",buffer_size:"<<buffer_size);

            waitcall.pop();
            donecall.push(cntl);

           // _stream->DelWriteTimeOutAsync(p);
           _stream->SetReadTimeOutAsync(p, _options.read_timeout_sec);

          //  p->SetReadTimeOut(_stream, _options.read_timeout_sec);
          buffer_size = cntl->_req_send_size - cntl->_req_pack_size;
        }
    }

    if(waitcall.empty())
    {
        WARN(_stream->GetSockName()<<",no wait call pending, send:"<<buffer_size);
        //stream->ModEventAsync(p, true, false);
        return;
    }
}

void RpcChannelImpl::OnMessageRecived(ConnectStream *sream, Epoller* p,int32_t buffer_size){

    _stream->DelReadTimeOutAsync(p);
    //p->DelReadTimeOut(_stream);

   if(donecall.empty())
   {
       INFO(sream->GetSockName()<<"no send donecall call");
       return;
   }

   RpcClientController * cntl = donecall.front();
    donecall.pop();

    /*if(cntl->_seq !=  sream->resheader.GetSeq()
            && sream->resheader.GetSeq() !=0 )
    {

        WARN("Seq Not Match,send_req:"<<cntl->_seq<<",res_header:"<<sream->resheader.toString());
        sream->resheader.Reset();
        OnMessageError(sream,p,CLIENT_SEQ_NOT_MATCH);
        cntl->SetStatus(CLIENT_SEQ_NOT_MATCH);
        return;
    }*/
    assert(cntl!=NULL);

    int ContentLength = sream->resheader.GetContentLength();
    TRACE("recive pocket:"<<sream->GetSockName()<<",buffer_size:"<<buffer_size<<",content:"<<ContentLength);

    sream->readbuf.BeginPacket();
    if(ContentLength >0
            && !cntl->_response->ParseFromZeroCopyStream(&sream->readbuf))
    {
        sream->resheader.Reset();
        OnMessageError(sream,p,SERVER_REQ_PARSER_FAILED);
        cntl->SetStatus(SERVER_REQ_PARSER_FAILED);
        return;
    }

    if(cntl->_done)
        cntl->_done->Run();

    //notify the call
    cntl->SetStatus(sream->resheader.status);

    sream->resheader.Reset();

    //send next
    if(!sendcall.empty()){
        RpcClientController* cntl = NULL;
        sendcall.pop(cntl);

        SendToServer(cntl->_method, cntl, cntl->_request);
        // wait the read time out
        _stream->SetWriteTimeOutAsync(_poller, _options.write_timeout_sec);
        waitcall.push(cntl);
    }

    //TRACE("Rest the resheader:"<<sream->resheader.toString());
}


void RpcChannelImpl::OnSendTimeout(SocketStream* sream,Epoller* p)
{
  //    if(sream->wtimernode.key > 0)
         OnMessageError(sream, p, CLIENT_SEND_TIMEOUT);
}

void RpcChannelImpl::OnRecvTimeout(SocketStream* sream,Epoller* p)
{
  //  if(sream->rtimernode.key > 0)
         OnMessageError(sream, p, CLIENT_RECV_TIMEOUT);
}

void RpcChannelImpl::OnMessageError(SocketStream* stream, Epoller* p, uint32_t error_code)
{
      WARN("ERROR:"<<stream->GetSockName()<<","<<ErrorString(error_code));

      // cntl,req,res, is malloc by client ~
      // so not free at this point
      RpcClientController *  cntl = NULL;
      {
          while(sendcall.pop(cntl))
          {
              cntl->SetStatus(CLIENT_CANCEL_BY_ERROR);
              cntl = NULL;
          }
      }
      {
          while(!waitcall.empty())
          {
              cntl = waitcall.front();
              cntl->SetStatus(error_code);
              waitcall.pop();
          }
      }
      {
          while(!donecall.empty())
          {
              cntl = donecall.front();
              cntl->SetStatus(error_code);
              donecall.pop();
          }
      }
      //2 send fail header
      stream->ModEventAsync(p, true, true);
      stream->_close_when_empty = true;
}



}
