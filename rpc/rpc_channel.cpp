#include "rpc_channel.h"
#include "rpc_channel_impl.h"

namespace mtrpc {

RpcChannel::RpcChannel(const RpcChannelOptions& options)
    : _impl(new RpcChannelImpl(options))
{
}

int RpcChannel::Connect(const std::string& server_ip,
        int32_t server_port)
{
      return _impl->Connect(server_ip,server_port);
}

RpcChannel::~RpcChannel()
{
     delete _impl;
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
