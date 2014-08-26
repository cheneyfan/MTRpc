#include <sstream>
#include <stdint.h>
#include <sys/types.h>
#include <ifaddrs.h>

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
#include "rpc_server_connect.h"

#include "common/serverstat.h"
namespace mtrpc {





RpcServerImpl::RpcServerImpl(const RpcServerOptions& options):
    acceptor(),
    _options(options),
    poller(new Epoller()),
    group (new WorkGroup()),
    _service_pool(new ServicePool())
{
}

RpcServerImpl::~RpcServerImpl()
{

}

bool RpcServerImpl::RegisterService(google::protobuf::Service* service,
                                    bool take_ownership)
{
    const google::protobuf::ServiceDescriptor * desc
            = service->GetDescriptor();
    // register for state
    for(int i =0;i<desc->method_count();++i)
    {
        const google::protobuf::MethodDescriptor * mdesc
                = desc->method(i);
        ServerState::registerStatus(mdesc->full_name());
    }


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

    //build in
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
    //acceptor.AddEventASync(poller,true,false);
    acceptor.SetEvent(true,false,0);
    poller->AddEvent(&acceptor,true,false);


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
    ServerConnect* stream = new ServerConnect(sockfd);

    stream->handlerGetServiceAndMethod =
            NewPermanentExtClosure(this,&RpcServerImpl::handlerGetServiceAndMethod);

    stream->Start(poller,group);



}

bool  RpcServerImpl::handlerGetServiceAndMethod(const std::string& method_full_name,
                                                google::protobuf::Service**service,
                                                google::protobuf::MethodDescriptor**method){


    std::string service_name;
    std::string method_name;

    if (!ParseMethodFullName(method_full_name, service_name, method_name))
    {
        return false;
    }

    ServiceBoard* service_board = _service_pool->FindService(service_name);

    if(service_board == NULL)
    {

        return false;
    }

    *service = service_board->Service();

    *method  = const_cast<google::protobuf::MethodDescriptor*>((*service)->GetDescriptor()->FindMethodByName(method_name));

    if(*method == NULL)
        return false;

    return true;
}


bool RpcServerImpl::ParseMethodFullName(const std::string& method_full_name,
                                        std::string& service_full_name,
                                        std::string& method_name)
{
    std::string::size_type pos = method_full_name.rfind('.');

    if (pos == std::string::npos)
        return false;

    service_full_name = method_full_name.substr(0, pos);
    method_name = method_full_name.substr(pos + 1);
    return true;
}


std::string RpcServerImpl::getHostIp()
{
    struct ifaddrs * if_addr = NULL;
    std::string ip = "";

    if(0 != getifaddrs(&if_addr))
    {
        return ip;
    }

    struct ifaddrs * if_addr_back = if_addr;

    for(; if_addr != NULL; if_addr = if_addr->ifa_next)
    {
        // not ipv4
        if (if_addr->ifa_addr->sa_family != AF_INET)
            continue;

        void * tmp = &((struct sockaddr_in *)if_addr->ifa_addr)->sin_addr;

        char buffer[256]={0};

        inet_ntop(AF_INET, tmp, buffer, INET_ADDRSTRLEN);

        ip = buffer;

        if(ip.size() == 0 || ip.find("127.0")== 0 || ip.find("10.") != 0)
        {
            WARN("not use foriegn "<<ip);
            continue;
        }

        break;
    }

    if(if_addr_back)
        freeifaddrs(if_addr_back);

    return ip;
}

}




