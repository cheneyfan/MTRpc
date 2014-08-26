#include "rpc_server.h"
#include "rpc_server_impl.h"

#include <sstream>
namespace  mtrpc {

RpcServer::RpcServer(const RpcServerOptions& options)
    : _impl(new RpcServerImpl(options))
{
}

RpcServer::~RpcServer()
{
    Stop();
    delete _impl;
}

bool RpcServer::Start(const std::string& server_address)
{
    return _impl->Start(server_address);
}

bool RpcServer::Start(uint32_t port)
{
    std::string ip = _impl->getHostIp();
    if(ip.size()==0)
         return false;
    std::stringstream str;
    str<<ip<<":"<<port;
    return _impl->Start(str.str());
}

void RpcServer::Stop()
{
    _impl->Stop();
}


int RpcServer::Join()
{
    _impl->Join();
    return 0;
}

RpcServerOptions RpcServer::GetOptions()
{
    return _impl->GetOptions();
}

void RpcServer::ResetOptions(const RpcServerOptions& options)
{
    _impl->ResetOptions(options);
}

bool RpcServer::RegisterService(google::protobuf::Service* service, bool take_ownership)
{
    return _impl->RegisterService(service, take_ownership);
}

int RpcServer::ServiceCount()
{
    return _impl->ServiceCount();
}

int RpcServer::ConnectionCount()
{
    return _impl->ConnectionCount();
}


}

