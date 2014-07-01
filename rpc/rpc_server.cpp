#include "rpc_server.h"
#include "rpc_server_impl.h"

namespace  mtrpc {

RpcServer::RpcServer(const RpcServerOptions& options)
    : _impl(new RpcServerImpl(options, handler))
{
}

RpcServer::~RpcServer()
{
    Stop();
}

bool RpcServer::Start(const std::string& server_address)
{
    return _impl->Start(server_address);
}

void RpcServer::Stop()
{
    _impl->Stop();
}


int RpcServer::Run()
{
    _impl->Run();
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

bool RpcServer::IsListening()
{
    return _impl->IsListening();
}

}

