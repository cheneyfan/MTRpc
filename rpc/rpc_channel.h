#ifndef _MTPRC_RPC_CHANNEL_H_
#define _MTPRC_RPC_CHANNEL_H_

#include <google/protobuf/service.h>
#include <smart_ptr/smart_ptr.hpp>


namespace mtrpc {
class RpcClient;
class RpcChannelImpl;


class RpcChannelOptions{
public:
      uint64_t connect_timeout;

      RpcChannelOptions():connect_timeout(10)
      {
      }
};


class RpcChannel : public google::protobuf::RpcChannel
{
public:
    // The "server_address" should be in format of "ip:port".
    RpcChannel(RpcClient* rpc_client,
               const std::string& server_address,
               const RpcChannelOptions& options = RpcChannelOptions());
    RpcChannel(RpcClient* rpc_client,
               const std::string& server_ip,
               uint32 server_port,
               const RpcChannelOptions& options = RpcChannelOptions());
    virtual ~RpcChannel();

    // Check the channel's address is valid.  If not valid, the following invoke
    // of "CallMethod()" will return RPC_ERROR_RESOLVE_ADDRESS.
    bool IsAddressValid();

    // Implements the google::protobuf::RpcChannel interface.  If the
    // "done" is NULL, it's a synchronous call, or it's asynchronous and
    // uses the callback to inform the completion (or failure).
    virtual void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                            ::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Message* request,
                            ::google::protobuf::Message* response,
                            ::google::protobuf::Closure* done);



private:
    RpcChannelImpl* _impl;
};

}
