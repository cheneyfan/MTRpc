#ifndef _MTPRC_RPC_CHANNEL_H_
#define _MTPRC_RPC_CHANNEL_H_

#include <google/protobuf/service.h>
#include <smart_ptr/smart_ptr.hpp>


namespace mtrpc {
class RpcClient;
class RpcChannelImpl;
class RpcController;

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
    RpcChannel(const RpcChannelOptions& options = RpcChannelOptions());
    virtual ~RpcChannel();

    ///
    /// \brief Connect
    /// \param server_ip
    /// \param server_port
    /// \return
    ///
    int Connect(const std::string& server_ip,int32_t server_port);

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


    ///
    /// \brief getController
    /// \return
    ///
    RpcController* GetController();

public:
    RpcChannelImpl* _impl;
};

}
#endif
