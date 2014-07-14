


#include <google/protobuf/service.h>

#include "thread/ext_closure.h"
#include <queue>

namespace mtrpc {


typedef ::google::protobuf::Message Message;

class RpcController;
class MessageStream;
class SocketStream;
class Epoller;

class ServerConnect
{
public:

    ServerConnect(int fd);

    ~ServerConnect();


    void Start(Epoller* p,WorkGroup* g);


    struct CallResult
    {
        Message* request;
        Message* reponse;
        RpcController* cntl;
    };

public:

    void OnMessageRecived(MessageStream* sream,Epoller* p,uint32_t buffer_size);

    void OnMessageSended(MessageStream* sream,Epoller* p,uint32_t buffer_size);

    void OnWriteable(SocketStream* sream,Epoller* p,uint32_t buffer_size);

    void OnMessageError(SocketStream* sream,Epoller* p,uint32_t error_code);


public:
    ExtClosure<bool(const std::string & fulllname,
                    google::protobuf::Service* &service,
                    google::protobuf::MethodDescriptor* &method
                    )> handlerGetServiceAndMethod;

    void SendMessage(MessageStream* sream,Epoller* p,RpcController*cntl,Message*req,Message*res);


public:
    MessageStream *_stream;
    Epoller* _poller;
    WorkGroup* _group;
    std::queue<CallResult> penging;


}


}
