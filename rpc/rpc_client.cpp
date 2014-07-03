#include "rpc_client.h"

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

}
