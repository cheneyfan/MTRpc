#ifndef _MTPRC_RPC_CHANNEL_IMPL_H_
#define _MTPRC_RPC_CHANNEL_IMPL_H_

#include <google/protobuf/service.h>
#include "rpc_client.h"


namespace mtrpc {
class RpcClient;
class RpcChannelImpl;
class ConnectStream;
class Epoller;

class RpcChannelImpl
{
public:

    RpcChannelImpl(const RpcClientOptions& options);

    virtual ~RpcChannelImpl();

    bool IsAddressValid();

    ///
    /// \brief Connect
    /// \param server_ip
    /// \param server_port
    /// \return
    ///
    int Connect(const std::string& server_ip,int32_t server_port);

    ///
    /// \brief CallMethod
    /// \param method
    /// \param controller
    /// \param request
    /// \param response
    /// \param done
    ///
    void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                    ::google::protobuf::RpcController* controller,
                    const ::google::protobuf::Message* request,
                    ::google::protobuf::Message* response,
                    ::google::protobuf::Closure* done);
public:
    RpcClientOptions _options;
    ConnectStream * _stream;
    Epoller* _poller;

};

}
