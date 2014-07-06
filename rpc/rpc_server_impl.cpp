#include "thread/closure.h"
#include "rpc_service_pool.h"

#include "rpc_server_impl.h"
#include "mio/mio_message_stream.h"

#include "thread/workpool.h"

#include "rpc_controller.h"

#include "mio/mio_poller.h"
#include "mio/mio_notify.h"

namespace mtrpc {


RpcServerImpl::RpcServerImpl(const RpcServerOptions& options):_options(options)
{

    poller = new Epoller();
    group = new WorkGroup();

    _service_pool = new ServicePool();
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

int RpcServerImpl::Start(const std::string& server_address)
{

    /// init work group
    group->Init(1);
    group->Post(NewExtClosure(poller,&Epoller::Poll));

    /// start listen
    acceptor.handerAccept = NewPermanentExtClosure(this,&RpcServerImpl::OnAccept);
    acceptor.StartListen(server_address);

    /// begin poll
    acceptor.AddEventASync(poller,true,false);

    return 0;

}

int RpcServerImpl::Join(){

    group->join();
    return 0;
}
void RpcServerImpl::OnAccept(int sockfd){


    // add new stream
    MessageStream* stream = new MessageStream(sockfd);

    stream->group = group;
    stream->handerMessageRecived = NewPermanentExtClosure(this,&RpcServerImpl::OnMessageRecived);

    stream->AddEventASync(poller,true,false);

}

void  RpcServerImpl::OnMessageRecived(MessageStream* stream,Epoller*p){

    std::string& method_full_name = stream->reqheader.path;
    std::string service_name;
    std::string method_name;

    if (!ParseMethodFullName(method_full_name, &service_name, &method_name))
    {
        return;
    }

    ServiceBoard* service_board = _service_pool->FindService(service_name);

    google::protobuf::Service* service = service_board->Service();

    const google::protobuf::MethodDescriptor* method =
            service->GetDescriptor()->FindMethodByName(method_name);

    const google::protobuf::Message* request
            = service->GetRequestPrototype(method).New();
    google::protobuf::Message* response
            = service->GetResponsePrototype(method).New();

    request->ParseFromZeroCopyStream(&stream->readbuf);

    google::protobuf::RpcController* controller = new RpcController();

    MethodBoard* method_board = service_board->Method(method->index());

    method_board->ReportProcessBegin();

    google::protobuf::Closure* done = NewClosure(this,
                &RpcServerImpl::OnCallMethodDone, controller, request, response,
                stream, p);

    service->CallMethod(method, controller, request, response, done);

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


    resheader.SetStatus(200);

    WriteBuffer::Iterator packetstart = buf.Reserve();

    WriteBuffer::Iterator itlast  = buf.writepos;

    response->SerializePartialToZeroCopyStream(&buf);

    stream->packetEnd = buf.writepos;

    resheader.content_length  = stream->packetEnd
            - itlast;

    resheader.SerializeReponseHeader(packetstart);

    stream->ModEventAsync(p, true, true);



    delete response;
    delete controller;
    delete request;

}


}




