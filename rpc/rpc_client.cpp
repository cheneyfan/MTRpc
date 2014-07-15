#include "rpc_client.h"
#include "rpc_client_impl.h"

namespace  mtrpc {


RpcClient::RpcClient(const RpcClientOptions& options)
    : _impl(new RpcClientImpl(options))
{
    _impl->Start();
}

RpcClient::~RpcClient()
{
    Shutdown();
}

RpcClientOptions RpcClient::GetOptions()
{
    return _impl->GetOptions();
}

void RpcClient::ResetOptions(const RpcClientOptions& options)
{
    _impl->ResetOptions(options);
}

int RpcClient::ConnectionCount()
{
    return _impl->ConnectionCount();
}

void RpcClient::Shutdown()
{
    _impl->Stop();
}


RpcChannel* RpcClient::GetChannel(const std::string& server_addr)
{
    return _impl->GetChannel(server_addr);
}



void RpcClient::ReleaseChannel(RpcChannel* channel)
{
    return _impl->ReleaseChannel(channel);
}

}
