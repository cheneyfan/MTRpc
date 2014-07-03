#include "rpc_channel.h"
#include <sstream>

namespace mtrpc {

RpcChannel::RpcChannel(RpcClient* rpc_client,
        const std::string& server_address,
        const RpcChannelOptions& options)
    : _impl(new RpcChannelImpl(rpc_client->impl(), server_address, options))
{
}

RpcChannel::RpcChannel(RpcClient* rpc_client,
        const std::string& server_ip,
        uint32 server_port,
        const RpcChannelOptions& options)
{
    std::ostringstream os;
    os << server_ip << ":" << server_port;
    _impl.reset(new RpcChannelImpl(rpc_client->impl(), os.str(), options));
}

RpcChannel::~RpcChannel()
{
}

bool RpcChannel::IsAddressValid()
{
    return _impl->IsAddressValid();
}

void RpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                            ::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Message* request,
                            ::google::protobuf::Message* response,
                            ::google::protobuf::Closure* done)
{
    _impl->CallMethod(method, controller, request, response, done);
}

} // namespace pbrpc
