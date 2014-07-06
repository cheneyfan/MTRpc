#ifndef _MTPRC_RPC_CHANNEL_IMPL_H_
#define _MTPRC_RPC_CHANNEL_IMPL_H_

#include <queue>


#include <google/protobuf/service.h>
#include "rpc_client.h"
#include "common/rwlock.h"

namespace mtrpc {
class RpcClient;
class RpcChannelImpl;
class ConnectStream;
class Epoller;
class MioTask;
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

    struct CallParams {
        ::google::protobuf::MethodDescriptor* method;
        ::google::protobuf::RpcController* controller;
        ::google::protobuf::Message* request;
        ::google::protobuf::Message* response;
        ::google::protobuf::Closure* done;
    };

    void OnCallDone(ConnectStream *_stream){

        // if pending call size > 0 ,pop and do

    }

   int SendToServer(::google::protobuf::MethodDescriptor* method,::google::protobuf::RpcController* controller,::google::protobuf::Message* request);
public:

  void OnMessageRecived(MessageStream* sream,Epoller* p);

  void OnMessageSended(MessageStream* sream,Epoller* p);


public:
    RpcClientOptions _options;
    ConnectStream * _stream;
    Epoller* _poller;


    MutexLock pendinglock;
    std::queue<CallParams*> pendingcall;

};

}
