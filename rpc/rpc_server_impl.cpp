#include "rpc_server_impl.h"
#include "mio/mio_message_stream.h"
#include "mio/mio_poller.h"
#include "thread/workpool.h"
#include "rpc_service_pool.h"
#include "rpc_controller.h"
#include "thread/closure.h"

namespace mtrpc {


RpcServerImpl::RpcServerImpl(const RpcServerOptions& options):_options(options)
{

    poller = new EPoller();
    group = new WorkGroup();

    pool = new ServicePool();
}


bool RpcServerImpl::RegisterService(google::protobuf::Service* service, bool take_ownership)
{
     return _service_pool->RegisterService(service, take_ownership);
}


RpcServerOptions RpcServerImpl::GetOptions()
{
    return _options;
}

void RpcServerImpl::ResetOptions(const RpcServerOptions& options)
{
    _options = options;
}


int RpcServerImpl::ServiceCount()
{
    return 0;
}

int RpcServerImpl::ConnectionCount()
{
    return 0;
}

bool RpcServerImpl::Start(const std::string& server_address)
{

    /// init work group
    group->Init(1);
    group->Post(NewExtClosure(poller,&Epoller::Poll));

    /// start listen
    acceptor.handerAccept = NewPermanentExtClosure(this,&RpcServerImpl::OnAccept,-1);
    acceptor.StartListen(server_address);

    /// begin poll
    acceptor.AddEventASync(poller,true,false);

}

void RpcServerImpl::OnAccept(int sockfd){


    // add new stream
    MessageStream* stream = new MessageStream(sockf);

    stream->group = group;
    stream->handerMessageRecived = NewPermanentExtClosure(this,&RpcServerImpl::OnMessageRecived,NULL);

    stream->AddEventASync(p,true,false);

}

int  RpcServerImpl::OnMessageRecived(MessageStream* stream,Epoller*p){

    std::string& method = stream->header.path;
    std::string service_name;
    std::string method_name;

    if (!ParseMethodFullName(method_full_name, &service_name, &method_name))
    {
        return -1;
    }

    ServiceBoard* service_board = _service_pool->FindService(service_name);

    google::protobuf::Service* service = service_board->Service();

    const google::protobuf::MethodDescriptor* method =
        service->GetDescriptor()->FindMethodByName(method_name);

    google::protobuf::Message* request
            = service->GetRequestPrototype(method).New();
    google::protobuf::Message* response
            = service->GetResponsePrototype(method).New();

    request->ParseFromZeroCopyStream(stream->readbuf);

    RpcController* controller = new RpcController();

    MethodBoard* method_board = service_board->Method(method->index());

    method_board->ReportProcessBegin();

    google::protobuf::Closure* done = NewClosure(
            &RpcServerImpl::OnCallMethodDone, controller, request, response,
            stream, p);
    service->CallMethod(method, controller, request, response, stream, done);

}


bool RpcServerImpl::ParseMethodFullName(const std::string& method_full_name,
        std::string* service_full_name, std::string* method_name)
{
    std::string::size_type pos = method_full_name.rfind('.');
    if (pos == std::string::npos) return false;
    *service_full_name = method_full_name.substr(0, pos);
    *method_name = method_full_name.substr(pos + 1);
    return true;
}


void RpcServerImpl::OnCallMethodDone(RpcController* controller,Message* request,Message* response,MessageStream* stream,Epoller* p){

    WriteBuffer& buf = stream->writebuf;
    HttpHeader& resheader = stream->resheader;

    resheader.status = 200;

    stream->packetstart = buf.Reserved();

    WriteBuffer::Iterator itlast  = buf.WritePos();

    response->SerializePartialToZeroCopyStream(buf);

    stream->packetend = buf.WritePos();

    resheader.content_length  = stream->packetend
            - itlast;

    resheader.SerializeHeader(buf, stream->packetstart);

    stream->ModEventAsync(p, true, true);

    stream->readbuf.trim();

    delete response;
    delete controller;
    delete request;

}


}



}
