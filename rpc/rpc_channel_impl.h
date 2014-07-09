#ifndef _MTPRC_RPC_CHANNEL_IMPL_H_
#define _MTPRC_RPC_CHANNEL_IMPL_H_

#include <queue>


#include <google/protobuf/service.h>
#include "rpc_channel.h"
#include "common/rwlock.h"

namespace mtrpc {
class RpcClient;
class RpcChannelImpl;
class ConnectStream;
class Epoller;
class WorkGroup;
class MessageStream;
class SocketStream;

class RpcChannelImpl
{
public:

    RpcChannelImpl(const RpcChannelOptions& options);

    virtual ~RpcChannelImpl();

    bool IsAddressValid();


    ///
    /// \brief getController
    /// \return
    ///
    RpcController* GetController();

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
        const ::google::protobuf::MethodDescriptor* method;
        ::google::protobuf::RpcController* controller;
        const ::google::protobuf::Message* request;
        ::google::protobuf::Message* response;
        ::google::protobuf::Closure* done;

        ~CallParams(){
            delete controller;
            delete done;
        }
    };

    void OnCallDone(ConnectStream *_stream){

        // if pending call size > 0 ,pop and do

    }

   int SendToServer(const google::protobuf::MethodDescriptor *method, ::google::protobuf::RpcController* controller, const google::protobuf::Message *request);
public:

  void OnMessageRecived(MessageStream* sream,Epoller* p);

  void OnMessageSended(MessageStream* sream,Epoller* p);


  void OnWriteable(SocketStream* sream,Epoller* p);

  void OnClose(SocketStream* sream,Epoller* p);

public:
    RpcChannelOptions _options;
    ConnectStream * _stream;
    Epoller* _poller;
    WorkGroup* _group;

    MutexLock pendinglock;
    std::queue<CallParams*> pendingcall;

};

}

#endif
