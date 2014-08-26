#ifndef _MTPRC_RPC_CHANNEL_IMPL_H_
#define _MTPRC_RPC_CHANNEL_IMPL_H_

#include <queue>


#include <google/protobuf/service.h>
#include "rpc_channel.h"
#include "common/rwlock.h"
#include "common/spinlist.h"

namespace mtrpc {
class RpcClient;
class RpcChannelImpl;
class ConnectStream;
class Epoller;
class WorkGroup;
class MessageStream;
class SocketStream;
class RpcClientController;

class RpcChannelImpl
{
public:

    RpcChannelImpl(const RpcChannelOptions& options);

    virtual ~RpcChannelImpl();

    bool IsAddressValid();


    ///
    /// \brief Connect
    /// \param server_ip
    /// \param server_port
    /// \return
    ///
    int Connect(const std::string& server_ip,int32_t server_port);



    RpcController* GetController();

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

    int SendToServer(const google::protobuf::MethodDescriptor *method, RpcClientController* controller, const google::protobuf::Message *request);

public:

    void OnMessageRecived(ConnectStream* sream,Epoller* p,int32_t buffer_size);

    void OnMessageSended(ConnectStream* stream, Epoller* p, int32_t buffer_size);

    void OnMessageError(SocketStream* stream, Epoller* p, uint32_t error_code);
    void OnClose(SocketStream* sream,Epoller* p);

    void OnWriteable(SocketStream* sream,Epoller* p);

public:
    RpcChannelOptions _options;
    ConnectStream * _stream;
    Epoller* _poller;
    WorkGroup* _group;

    //Use SpinList make sure thread safety
    SpinList<RpcClientController*,MutexLock> sendcall;

    std::queue<RpcClientController*> waitcall;

};

}

#endif
