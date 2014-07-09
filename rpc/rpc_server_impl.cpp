#include <sstream>

#include "thread/closure.h"
#include "rpc_service_pool.h"

#include "rpc_server_impl.h"
#include "mio/mio_message_stream.h"

#include "thread/workpool.h"

#include "rpc_controller.h"

#include "mio/mio_poller.h"
#include "mio/mio_notify.h"

#include "proto/builtin_service_impl.h"
#include "log/log.h"

namespace mtrpc {





RpcServerImpl::RpcServerImpl(const RpcServerOptions& options):
    acceptor(),
    _options(options)
{

    poller = new Epoller();
    group = new WorkGroup();

    _service_pool = new ServicePool();


}

RpcServerImpl::~RpcServerImpl()
{

}

bool RpcServerImpl::RegisterService(google::protobuf::Service* service, bool take_ownership)
{
    std::stringstream str;

    const google::protobuf::ServiceDescriptor * desc = service->GetDescriptor();
/*
    str <<"Service:"<<desc->full_name()<<",method_num:"<<desc->method_count()<<std::endl;

    for(int i = 0;i < desc->method_count();++i)
    {
        const google::protobuf::MethodDescriptor * mdesc = desc->method(i);
        str<<"    method:"<<mdesc->full_name()
          <<",req:"<<mdesc->input_type()->full_name()
         <<",res:"<<mdesc->output_type()->full_name()<<std::endl;
    }

    INFO("Regiter: "<<str.str());
    */

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

    RegisterService(new builtin::BuiltinServiceImpl());

    /// init work group
    group->Init(_options.work_thread_num);

    /// start listen
    acceptor.handerAccept = NewPermanentExtClosure(this,&RpcServerImpl::OnAccept);

    int ret = acceptor.StartListen(server_address);

    if(ret !=0)
    {
        ERROR("server listen failed:"<<server_address<<",worker num:"<<_options.work_thread_num);
        return -1;
    }

    /// begin poll
    acceptor.AddEventASync(poller,true,false);


    group->Post(NewExtClosure(poller,&Epoller::Poll));


    INFO("server listen at:"<<server_address<<",worker num:"<<_options.work_thread_num);
    return 0;

}

int RpcServerImpl::Join(){

    group->join();
    return 0;
}


void RpcServerImpl::Stop(){

    poller->Stop();
    group->Stop();
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

    TRACE("OnMessageRecived fullname:"<<method_full_name<<",service_name:"<<service_name<<",method_name:"<<method_name);

    ServiceBoard* service_board = _service_pool->FindService(service_name);

    google::protobuf::Service* service = service_board->Service();

    const google::protobuf::MethodDescriptor* method =
            service->GetDescriptor()->FindMethodByName(method_name);

    google::protobuf::Message* request
            = service->GetRequestPrototype(method).New();
    google::protobuf::Message* response
            = service->GetResponsePrototype(method).New();



    bool ret = request->ParseFromZeroCopyStream(&stream->readbuf);

    TRACE("req:"<<request->GetDescriptor()->full_name()
          <<",res:"<<request->GetDescriptor()->full_name()
          <<",ret:"<<ret
          <<",rpos:"<<stream->readbuf.readpos.toString()
          <<",wpos:"<<stream->readbuf.writepos.toString());

    google::protobuf::RpcController* controller = new RpcController();

    MethodBoard* method_board = service_board->Method(method->index());

    method_board->ReportProcessBegin();

    google::protobuf::Closure* done = NewClosure(this,
                                                 &RpcServerImpl::OnCallMethodDone,
                                                 (RpcController*)controller,
                                                 request,
                                                 response,
                                                 stream,
                                                 p);

    service->CallMethod(method, controller, request, response, done);

    method_board->ReportProcessEnd(true,1);

}


bool RpcServerImpl::ParseMethodFullName(const std::string& method_full_name,
                                        std::string* service_full_name, std::string* method_name)
{
    std::string::size_type pos = method_full_name.rfind('.');

    if (pos == std::string::npos)
        return false;

    *service_full_name = method_full_name.substr(0, pos);
    *method_name = method_full_name.substr(pos + 1);
    return true;
}


void RpcServerImpl::OnCallMethodDone(RpcController* controller,const google::protobuf::Message* request,google::protobuf::Message* response,MessageStream* stream,Epoller* p){

    WriteBuffer& buf = stream->writebuf;
    HttpHeader& resheader = stream->resheader;

    resheader.SetStatus(200);

    WriteBuffer::Iterator packetstart = buf.Reserve();

    WriteBuffer::Iterator itlast  = buf.writepos;

    response->SerializeToZeroCopyStream(&buf);

    stream->packetEnd = buf.writepos;

    uint32_t content_length  = stream->packetEnd - itlast;

    resheader.SetContentLength(content_length);

    resheader.SerializeReponseHeader(packetstart);

    stream->ModEventAsync(p, true, true);

    TRACE("send header:"<<packetstart.get()->buffer
          <<",rpos"<<buf.readpos.toString()
          <<",wpos"<<buf.writepos.toString());


    delete request;
    delete response;
    delete controller;


}


}




