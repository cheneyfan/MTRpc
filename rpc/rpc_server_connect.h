#ifndef _MTRPC_RPC_SERVER_CONNECT_H_
#define _MTRPC_RPC_SERVER_CONNECT_H_


#include <google/protobuf/service.h>

#include "thread/ext_closure.h"
#include <queue>

namespace mtrpc {


typedef ::google::protobuf::Message Message;

class RpcServerController;
class MessageStream;
class SocketStream;
class Epoller;
class WorkGroup;

class ServerConnect
{
public:

    ServerConnect(int fd);

    ~ServerConnect();


    void Start(Epoller* p,WorkGroup* g);

    void SendMessage(MessageStream* sream, Epoller* p, RpcServerController*cntl, Message*request, Message*response);

public:

    void OnMessageRecived(MessageStream* stream, Epoller* p, int32_t buffer_size);

    void OnMessageSended(MessageStream* stream, Epoller* p,int32_t buffer_size);

    void OnMessageError(SocketStream* stream, Epoller* p, uint32_t error_code);

    void OnClose(SocketStream* sream,Epoller* p);


public:

    ExtClosure<bool(const std::string & fulllname,
                    google::protobuf::Service** service,
                    google::protobuf::MethodDescriptor** method
                    )>*  handlerGetServiceAndMethod;

public:

    MessageStream *_stream;
    Epoller* _poller;
    WorkGroup* _group;
    std::queue<RpcServerController*> pending;


};

}
#endif
