#include "rpc_server_connect.h"
#include "rpc_controller_impl.h"
#include "mio/mio_message_stream.h"
#include "mio/mio_error_code.h"
#include "log/log.h"
#include "mio/mio_poller.h"

namespace mtrpc {

ServerConnect::ServerConnect(int fd):
_stream(new MessageStream(fd))
{
}


void ServerConnect::Start(Epoller* p,WorkGroup* g)
{
    _poller = p;
    _group = g;
    _stream->group = g;

    _stream->AddEventASync(p,true,false);

}

void ServerConnect::OnMessageRecived(MessageStream* sream,Epoller* p,uint32_t buffer_size)
{
    const std::string& method_full_name = _stream->reqheader.path;


    google::protobuf::Service* service = NULL;
    google::protobuf::MethodDescriptor* method = NULL;

    if(!handlerGetServiceAndMethod(method_full_name,service,method))
    {
        OnMessageError(sream,p,HTTP_SERVER_NOTFOUND);
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

    google::protobuf::RpcController* controller = new RpcController();


    service->CallMethod(method, controller, request, response, NULL);

    if(controller->Failed()){

        OnMessageError(sream,p,SERVER_REQ_PARSER_FAILED);
    }

    SendMessage(stream,p,cntl,request,reponse);
}

void ServerConnect::SendMessage(MessageStream* stream,Epoller* p,RpcController*cntl,Message*req,Message*res){


    if(!penging.empty())
    {

        CallResult cr;
        cr.cntl=cntl;
        cr.request =req;
        cr.res;
        penging.push(cr);
        return cr;
    }

    WriteBuffer& buf = stream->writebuf;

    HttpRequestHeader& reqheader = stream->reqheader;
    HttpReponseHeader& resheader = stream->resheader;

    resheader.SetSeq(reqheader.GetSeq());
    resheader.SetStatus(OK,"OK");
    if(! buf.Reserve(MAX_HEADER_SIZE))
    {
        OnMessageError(stream,p,SERVER_WRIEBUFFER_FULL);
        delete cntl;
        delete req;
        delete res;
        return;
    }

     WriteBuffer::Iterator packetStart = buf.writepos;

    WriteBuffer::Iterator bodyStart =  buf.AlignWritePos();

    if(!response->SerializeToZeroCopyStream(&buf))
    {
        OnMessageError(stream,p,SERVER_RES_SERIA_FAILED);
        delete cntl;
        delete req;
        delete res;
        return;

    }

    uint32_t content_length  = buf.writepos - bodyStart;

    resheader.SetContentLength(content_length);
    resheader.SerializeHeader(packetStart);
stream->ModEventAsync(p, true, true);

delete cntl;
delete req;
delete res;



}



}
