#include "rpc_server_connect.h"
#include "rpc_controller_impl.h"
#include "mio/mio_message_stream.h"
#include "mio/mio_error_code.h"
#include "log/log.h"
#include "mio/mio_poller.h"
#include "common/timerhelper.h"
#include "common/serverstat.h"
#include "google/protobuf/message.h"

#define HTTP_MAX_REQUEST_PENDING 5


namespace mtrpc {

ServerConnect::ServerConnect(int fd):
    handlerGetServiceAndMethod(NULL),
_stream(new MessageStream(fd))
{
    _stream->RequireRef();
}


ServerConnect::~ServerConnect()
{
    delete handlerGetServiceAndMethod;
    handlerGetServiceAndMethod = NULL;

    delete handlerGetServiceList;
    handlerGetServiceList = NULL;
}


void ServerConnect::Start(Epoller* p,WorkGroup* g)
{
    _poller = p;
    _group  = g;

    if(g->workerNum > 1)
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

    //TRACE("accept :"<<_stream->GetSockName());

}


void ServerConnect::OnListServer(MessageStream* stream, Epoller* p){

    // 1 get the register servers

    std::list<std::string> servers;
    handlerGetServiceList->Run(servers);

    RpcServerController* controller = new RpcServerController();


    controller->_seq  = stream->reqheader.GetSeq();
    controller->_request = NULL;
    controller->_response = NULL;
    controller->_stream = _stream;
    controller->_poller = p;
    controller->SetStatus(OK);

    if(servers.size() == 0)
    {
        controller->body = "No Serves find\r\n";
    }else{
         std::stringstream str;
         for(std::list<std::string>::iterator it = servers.begin();
             it != servers.end(); ++it)
         {
             str << *it;
         }
         controller->body = str.str();
    }

    SendMessage(stream, p, controller,NULL,NULL);
}

void ServerConnect::OnMessageRecived(MessageStream* sream,Epoller* p,int32_t buffer_size)
{

    if(sream->reqheader.path.size() <= 1)
    {

        TRACE(sream->GetSockName()<<",path:"<<sream->reqheader.path<<",buffer size:"<<buffer_size);

        OnListServer(sream, p);
        return;
    }

    const std::string& method_full_name = sream->reqheader.path.substr(1);

    TRACE(sream->GetSockName()<<",method:"<<method_full_name<<"Seq:"<<sream->reqheader.GetSeq());

    if(method_full_name.size() == 0)
    {
        OnMessageError(sream,p,HTTP_SERVER_NOTFOUND);
        WARN(sream->GetSockName()<<"not found server:"<<method_full_name);
        return;
    }

    google::protobuf::Service* service = NULL;
    google::protobuf::MethodDescriptor* method = NULL;

    if(!handlerGetServiceAndMethod->Run(method_full_name,&service,&method)
            || service == NULL
            || method == NULL)
    {
        OnMessageError(sream,p,HTTP_METHOD_NOTFOUND);
        WARN(sream->GetSockName()<<"not found method:"<<method_full_name);
        return;
    }

    google::protobuf::Message* request
            = service->GetRequestPrototype(method).New();

    _stream->readbuf.BeginPacket();

    TRACE(sream->GetSockName()<<"begin,read:"<<_stream->readbuf.readpos.toString()<<",write:"<<_stream->readbuf.writepos.toString());

    if(!request->ParseFromZeroCopyStream(&_stream->readbuf))
    {
        WARN(sream->GetSockName()<<"paser error,read:"<<_stream->readbuf.readpos.toString()<<",beginRead:"<<_stream->readbuf.begin().toString());
        OnMessageError(sream,p,SERVER_REQ_PARSER_FAILED);
        delete request;
        return;

    }

    google::protobuf::Message* response
            = service->GetResponsePrototype(method).New();

    RpcServerController* controller = new RpcServerController();


    controller->_seq  = sream->reqheader.GetSeq();
    controller->_request = request;
    controller->_response = response;
    controller->_stream = _stream;
    controller->_poller = p;

    controller->SetStatus(OK);

    {
        ServerState::StateTimeMs _m(method->full_name(),false);
        service->CallMethod(method, controller, request, response, NULL);
        uint64_t elapse =  _m.Elapse();

        //TRACE(sream->GetSockName()<<"call result:"<<controller->_msg);
        INFO(method->full_name()<<",seq:"<<controller->_seq
             <<",result:"<<controller->_msg<<",time:"<<elapse<<",sock:"<<sream->GetSockName());
    }

    if(controller->Failed()){

        delete controller;
        delete request;
        delete response;
        OnMessageError(sream,p,SERVER_REQ_PARSER_FAILED);
        return ;
    }

    SendMessage(_stream,p,controller,request,response);

}

void ServerConnect::SendMessage(MessageStream* stream,Epoller* p,RpcServerController*cntl,Message*request,Message*response)
{

    if(!pending.empty() )
    {
        pending.push(cntl);
        stream->ModEventAsync(p, true, true);
        TRACE(stream->GetSockName()<<",pending:"<<pending.size());
        return ;
    }

    if(pending.size() > HTTP_MAX_REQUEST_PENDING)
    {
        OnMessageError(stream, p, HTTP_REQUEST_OVERFLOW);
        return;
    }

    //pending
    pending.push(cntl);

    WriteBuffer& buf = stream->writebuf;
    buf.Reset();

    HttpReponseHeader& resheader = stream->resheader;

    resheader.Reset();
    resheader.SetSeq(cntl->_seq);
    resheader.SetStatus( OK, "OK");

    //TRACE("send Seq:"<<cntl->_seq);

    if(request){

        if(!buf.Reserve(MAX_HEADER_SIZE))
        {
            OnMessageError(stream,p,SERVER_WRIEBUFFER_FULL);

            delete request;
            delete response;
            delete cntl;
            return;
        }

        WriteBuffer::Iterator packetStart = buf.writepos;
        WriteBuffer::Iterator bodyStart   =  buf.AlignWritePos();

        buf.BeginPacket();

        TRACE("Serial buf,write:"<<bodyStart.toString()<<",read:"<<buf.readpos.toString()<<",used:"<<buf.GetBufferUsed()<<",left:"<<buf.GetBufferLeft());

        if(!response->SerializeToZeroCopyStream(&buf))
        {
            //roll back
            buf.writepos = packetStart;
            OnMessageError(stream,p,SERVER_RES_SERIA_FAILED);

            delete request;
            delete response;
            delete cntl;
            return;
        }

        uint32_t content_length  = buf.writepos - bodyStart;

        resheader.SetContentLength(content_length);
        resheader.SetHeader("Content-Type"," application/x-protobuf");
        resheader.SerializeHeader(packetStart);

        cntl->_res_pack_size     = buf.writepos - packetStart;

        TRACE("send,readpos:"<<buf.readpos.toString()<<",writepos:"<<buf.writepos.toString()<<",packetStart:"<<packetStart.toString()<<",body start:"<<bodyStart.toString()<<",beginpacket:"<<buf.beginWrite.toString()<<",pack size:"<<cntl->_res_pack_size<<",content_length:"<<content_length);

    }else{

        resheader.body_size = cntl->body.size();

        resheader.SetHeader("Server"," MTRpc 1.0.0");
        //resheader.SetHeader("Date",
        //                    " Wed, 03 Dec 2014 08:32:40 GMT");
        resheader.SetHeader("Content-Type"," text/plain");
        resheader.SetHeader("Connection"," keep-alive");
        resheader.SetContentLength(resheader.body_size);

        TRACE("SerializeBody begin body_size:"<<resheader.body_size<<", writepos:"<<buf.writepos.toString());


        if(!buf.Reserve(MAX_HEADER_SIZE))
        {
            OnMessageError(stream,p,SERVER_WRIEBUFFER_FULL);

            delete request;
            delete response;
            delete cntl;
            return;
        }

        WriteBuffer::Iterator packetStart = buf.writepos;
        buf.AlignWritePos();

        resheader.SerializeBody(cntl->body, buf.writepos);
        resheader.SerializeHeader(packetStart);


        TRACE("SerializeBody after body_size:"<<resheader.body_size<<", writepos:"<<buf.writepos.toString());
    }

    stream->ModEventAsync(p, true, true);

}

void ServerConnect::OnMessageSended(MessageStream* stream, Epoller* p,int32_t buffer_size)
{
    if(pending.empty() )
    {
        stream->ModEventAsync(p,true,false);
        return;
    }

    if(stream->_close_when_empty)
    {
        TRACE(stream->GetSockName()<<" _close_when_empty send cancel");
        return;
    }

    RpcServerController* cntl = pending.front();

   TRACE(stream->GetSockName()<<",OnMessageSended,send:"<<cntl ->_res_send_size<<",pack:"<<cntl ->_res_pack_size<<",buffer_size:"<<buffer_size);

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

    cntl = pending.front();
    SendMessage(stream,p,cntl,cntl->_request,cntl->_response);
}


void ServerConnect::OnMessageError(SocketStream* stream, Epoller* p, uint32_t error_code)
{
     // 1 remove all pending
      WARN("ERROR:"<<stream->GetSockName()<<","<<ErrorString(error_code));

      RpcServerController* cntl = NULL;
      if(!pending.empty())
      {
          cntl = pending.front();
          //pending.pop();
      }

      //2 send fail header
      WriteBuffer::Iterator packetStart = stream->writebuf.writepos;
      WriteBuffer::Iterator bodyStart   = stream->writebuf.AlignWritePos();
      MessageStream* ms = (MessageStream*)stream;

      ms->resheader.Reset();

      if(cntl)
          ms->resheader.SetSeq(cntl->_seq);
      else
          ms->resheader.SetSeq(0);

      ms->resheader.SetStatus(error_code, ErrorString(error_code));
      ms->resheader.SetContentLength(0);
      ms->resheader.SerializeHeader(packetStart);
      stream->ModEventAsync(p, true, true);

      stream->_close_when_empty = true;

      /* defer release the cntl;
      if(cntl){
          delete cntl->_request;
          delete cntl->_response;
          delete cntl;
      }*/
}

void ServerConnect::OnClose(SocketStream* sream,Epoller* p){

    while(!pending.empty())
    {
        RpcServerController* cntl = pending.front();
        pending.pop();
        TRACE(_stream->GetSockName()<<",release cntl error:"<<cntl->ErrorCode()<<",seq:"<<cntl->_seq);
        delete cntl->_request;
        delete cntl->_response;
        delete cntl;

    }

     TRACE(_stream->GetSockName()<<",ServerConnect down!");
    _stream->ReleaseRef();

    delete this;
}



}
