#include "rpc_server_impl.h"

namespace mtrpc {


RpcServerImpl::RpcServerImpl(const RpcServerOptions& options):_options(options)
{
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


bool RpcServerImpl::IsListening(){
    return acceptor.isListening();

}




}
