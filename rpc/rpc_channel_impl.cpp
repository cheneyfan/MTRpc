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
        if(_stream->_ConnectStatus == CLIENT_CONNECT_OK)
        {
            _stream->OnClose(_poller);
        }
        //_stream->ReleaseRef();
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

    //_stream->handerMessageSended =
    //        NewPermanentExtClosure(this,&RpcChannelImpl::OnMessageSended);

    _stream->handerClose  =
            NewPermanentExtClosure(this,&RpcChannelImpl::OnClose);

    _stream->handerWriteable =
            NewPermanentExtClosure(this,&RpcChannelImpl::OnWriteable);


    int ret = _stream->Connect(server_ip, server_port);

    if( ret == CLIENT_CONNECT_OK)
    {
        _stream->AddEventASync(_poller,true,false);
        return 0;
    }

    if(ret != CLIENT_CONNECT_IPROCESS)
    {
        _stream->ReleaseRef();
        _stream = NULL;
        return -1;
    }

    _stream->AddEventASync(_poller,true,true);
    _stream->Wait();

    if( _stream->_ConnectStatus == CLIENT_CONNECT_OK)
    {
        TRACE("connect ok:"<<server_ip<<","<<server_port);
        return 0;
    }

    //Will delete by close
    return -1;
}




void RpcChannelImpl::OnClose(SocketStream *sream, Epoller *p){

    //
    //TODO reset sream
    //delete this;
    TRACE("close:"<<sream->GetSockName()<<",send:"<<sendcall.empty()<<",wait:"<<waitcall.size());

    if(!sendcall.empty() || !waitcall.empty())
        OnMessageError(sream,p,CLIENT_CANCEL_BY_CLOSE);

    _stream->ReleaseRef();
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
        _stream->ModEventAsync(_poller,true,true);
        //SendToServer(method, (RpcClientController*)controller, request);
    }

    //sync
    if(done == NULL)
        cntl->Wait();
}

void RpcChannelImpl::OnWriteable(SocketStream* sream,Epoller* p)
{

    RpcClientController* cntl = NULL;
    if(!sendcall.pop(cntl))
        return ;

    SendToServer(cntl->_method,cntl,cntl->_request);
    waitcall.push(cntl);
}

int RpcChannelImpl::SendToServer(const ::google::protobuf::MethodDescriptor* method,
                                 RpcClientController* cntl,
                                 const ::google::protobuf::Message* request){

    HttpRequestHeader& reqheader = _stream->reqheader;
    reqheader.Reset();

    WriteBuffer& buf = _stream->writebuf;
    buf.Reset();

    reqheader.SetPath(method->full_name());

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

    //TRACE("packetStart:"<<packetStart.toString()<<",size:"<<packetStart.get()->size<<",length:"<<strlen(packetStart.get()->buffer+packetStart._pos));

    reqheader.SerializeHeader(packetStart);
    TRACE("packetStart:"<<packetStart.toString()<<",size:"<<packetStart.get()->size<<",length:"<<strlen(packetStart.get()->buffer+packetStart._pos));

    cntl->_req_pack_size = buf.writepos - packetStart;

    TRACE("send "<<cntl->_req_pack_size <<",header:"<<packetStart.get()->buffer);

    _stream->ModEventAsync(_poller,true,true);

    return 0;
}

void RpcChannelImpl::OnMessageSended(ConnectStream* stream,Epoller* p,int32_t buffer_size){

    RpcClientController *  cntl = NULL;

    do{

        cntl = NULL;


        if(!sendcall.pop(cntl) )
        {
            stream->ModEventAsync(p,true,false);
            return;
        }





       // TRACE(stream->GetSockName()<<",send:"<<cntl ->_req_send_size<<",pack:"<<cntl ->_req_pack_size<<",buffer_size:"<<buffer_size);

        cntl ->_req_send_size  += buffer_size;

        if(cntl ->_req_send_size < cntl ->_req_pack_size)
        {
            return ;
        }

        waitcall.push(cntl);
        buffer_size =  cntl ->_req_send_size - cntl ->_req_pack_size;

    }while(buffer_size>0);
}

void RpcChannelImpl::OnMessageRecived(ConnectStream *sream, Epoller* p,int32_t buffer_size){

   if(waitcall.empty())
   {
       INFO(sream->GetSockName()<<"no waiting call");
       return;
   }

    RpcClientController * cntl = waitcall.front();
    waitcall.pop();

    if(cntl->_seq !=  sream->resheader.GetSeq()
            && sream->resheader.GetSeq() !=0 )
    {

        WARN("Seq Not Match,send_req:"<<cntl->_seq<<",res_header:"<<sream->resheader.toString());
        sream->resheader.Reset();
        OnMessageError(sream,p,CLIENT_SEQ_NOT_MATCH);
        cntl->SetStatus(CLIENT_SEQ_NOT_MATCH);
        return;
    }
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
    //TRACE("Rest the resheader:"<<sream->resheader.toString());
    cntl = NULL;
}


void RpcChannelImpl::OnMessageError(SocketStream* stream, Epoller* p, uint32_t error_code)
{
      WARN("ERROR:"<<stream->GetSockName()<<","<<ErrorString(error_code));

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

      //2 send fail header
      stream->ModEventAsync(p, true, true);
      stream->_close_when_empty = true;
}



}
