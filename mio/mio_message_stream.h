
#include "mio_socket_stream.h"

#include <google/protobuf/message.h>
#include "proto/rpc_http_header.h"

namespace mtrpc {

typedef ::google::protobuf::Message Message;

class MessageStream  :public SocketStream {

public :

    virtual int OnRecived(Epoller* p);
    virtual int OnSended(Epoller* p);



    virtual int OnMessage(Epoller* p);
public:

    Message* req;
    Message* res;
    HttpHeader header;
};
}
