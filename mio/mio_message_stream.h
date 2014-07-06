
#include "mio_socket_stream.h"

#include <google/protobuf/message.h>
#include "proto/rpc_http_header.h"
#include "thread/ext_closure.h"
namespace mtrpc {

typedef ::google::protobuf::Message Message;

class MessageStream  :public SocketStream {

public :


    MessageStream(int sockfd);


    ///
    /// \brief OnRecived
    /// \param p
    /// \return
    ///
    virtual int OnRecived(Epoller* p);
    virtual int OnSended(Epoller* p);
    virtual int OnConnect(Epoller *p);


public:

    ExtClosure<void(MessageStream* sream, Epoller* p)>*  handerConnected;

    ExtClosure<void(MessageStream* sream,Epoller* p)>*  handerMessageRecived;

    ExtClosure<void(MessageStream* sream,Epoller* p)>*  handerMessageSended;

    HttpHeader reqheader;
    HttpHeader resheader;

};
}
