#include "rpc_server_connect.h"
#include "rpc_controller_impl.h"
#include "mio/mio_message_stream.h"
#include "mio/mio_error_code.h"
#include "log/log.h"
#include "mio/mio_poller.h"

namespace mtrpc {

ServerConnect::ServerConnect(int fd):
_stream(new MessageStream(fd)),
handlerGetServiceAndMethod(NULL)
{
    _stream->RequireRef();
}


ServerConnect::~ServerConnect()
{
    delete handlerGetServiceAndMethod;
}


void ServerConnect::Start(Epoller* p,WorkGroup* g)
{
    _poller = p;
    _group  = g;
    _stream->group = g;

    _stream->handerMessageRecived  =
            NewPermanentExtClosure(this,&ServerConnect::OnMessageRecived);
    _stream->handerClose =
            NewPermanentExtClosure(this,&ServerConnect::OnClose);

    _stream->handerMessageSended =
            NewPermanentExtClosure(this,&ServerConnect::OnMessageSended);

    _stream->handerMessageError =
            NewPermanentExtClosure(this,&ServerConnect::OnMessageError);

    _stream->AddEventASync(p,true,false);
}

void ServerConnect::OnMessageRecived(MessageStream* sream,Epoller* p,uint32_t buffer_size)
{
    const std::string& method_full_name = sream->reqheader.path;

    if(method_full_name.size() == 0)
    {
        OnMessageError(sream,p,HTTP_SERVER_NOTFOUND);
        WARN("not found server:"<<method_full_name);
        return;
    }

    google::protobuf::Service* service = NULL;
    google::protobuf::MethodDescriptor* method = NULL;

    if(!handlerGetServiceAndMethod(method_full_name,&service,&method)
            || service == NULL
            || method == NULL)
    {
        OnMessageError(sream,p,HTTP_METHOD_NOTFOUND);
        WARN("not found method:"<<method_full_name);
        return;
    }

    google::protobuf::Message* request
            = service->GetRequestPrototype(method).New();

    if(!request->ParseFromZeroCopyStream(&_stream->readbuf))
    {
        OnMessageError(sream,p,SERVER_REQ_PARSER_FAILED);
        delete request;
        return;

    }

    google::protobuf::Message* response
            = service->GetResponsePrototype(method).New();

    RpcController* controller = new RpcController();

    controller->_request = request;
    controller->_request = response;
    controller->_stream = _stream;
    controller->_poller = p;


    service->CallMethod(method, controller, request, response, NULL);

    if(controller->Failed()){

        delete cntl;
        delete req;
        delete res;
        OnMessageError(sream,p,SERVER_REQ_PARSER_FAILED);
        return ;
    }

    _stream->reqheader.Reset();
    _stream->resheader.Reset();

    SendMessage(stream,p,cntl,request,reponse);

}

void ServerConnect::SendMessage(MessageStream* stream,Epoller* p,RpcController*cntl,Message*req,Message*res)
{

    if(!pending.empty() )
    {
        pending.push(cntl);
        return cr;
    }

    //pending
    pending.push(cntl);

    WriteBuffer& buf = stream->writebuf;

    HttpRequestHeader& reqheader = stream->reqheader;
    HttpReponseHeader& resheader = stream->resheader;

    resheader.Reset();
    resheader.SetSeq(reqheader.GetSeq());
    resheader.SetStatus( OK, "OK");

    if(!buf.Reserve(MAX_HEADER_SIZE))
    {
        OnMessageError(stream,p,SERVER_WRIEBUFFER_FULL);

        delete req;
        delete res;
        delete cntl;
        return;
    }

    WriteBuffer::Iterator packetStart = buf.writepos;
    WriteBuffer::Iterator bodyStart   =  buf.AlignWritePos();

    if(!response->SerializeToZeroCopyStream(&buf))
    {
        //roll back
        buf.writepos = packetStart;
        OnMessageError(stream,p,SERVER_RES_SERIA_FAILED);

        delete req;
        delete res;
        delete cntl;
        return;
    }

    cntl->_res_pack_size     = buf.writepos - packetStart;

    uint32_t content_length  = buf.writepos - bodyStart;

    resheader.SetContentLength(content_length);
    resheader.SerializeHeader(packetStart);

    stream->ModEventAsync(p, true, true);

}


void ServerConnect::OnMessageSended(MessageStream* stream, Epoller* p,uint32_t buffer_size)
{
    if(pending.empty() )
    {
        stream->ModEventAsync(p,true,false);
        return;
    }

    RpcController*cntl = pending.front();
    cntl ->_res_send_size  += buffer_size;

    if(cntl ->_res_send_size < cntl ->_res_pack_size)
    {
        return ;
    }

    delete cntl->_request;
    delete cntl->_response;
    delete cntl;
    pending.pop();

    if(pending.empty())
    {
        stream->ModEventAsync(p,true,false);
        return;
    }

    cntl =pending.front();
    SendMessage(stream,p,cntl,cntl->_request,cntl->_response);
}


void ServerConnect::OnMessageError(SocketStream* stream, Epoller* p, uint32_t error_code)
{
     // 1 remove all pending
      while(!penging.empty())
      {
          RpcController* cntl = penging.front();
          penging.pop();

          delete cntl->_request;
          delete cntl->_response;
          delete cntl;
      }

      //2 send fail header
      WriteBuffer::Iterator packetStart = stream->writebuf.writepos;
      WriteBuffer::Iterator bodyStart   = stream->writebuf.AlignWritePos();
      MessageStream* ms = (MessageStream*)stream;

      ms->resheader.Reset();
      ms->resheader.SetSeq(reqheader.GetSeq());
      ms->resheader.SetStatus(error_code, ErrorString(error_code));
      ms->resheader.SetContentLength(0);
      ms->resheader.SerializeHeader(packetStart);
      stream->ModEventAsync(p, true, true);
      stream->_close_when_empty = true;
}

void ServerConnect::OnClose(SocketStream* sream,Epoller* p){

    while(!penging.empty())
    {
        RpcController* cntl = penging.front();
        penging.pop();

        delete cntl->_request;
        delete cntl->_response;
        delete cntl;
    }

    _stream->ReleaseRef();
    TRACE("ServerConnect down!");
    delete this;
}



}
