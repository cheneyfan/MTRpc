
#include "mio_socket_stream.h"

#include <google/protobuf/message.h>
#include "proto/rpc_http_header.h"
#include "thread/ext_closure.h"
namespace mtrpc {

typedef ::google::protobuf::Message Message;

///
/// \brief The MessageStream class
///
class MessageStream  :public SocketStream {

public :

    MessageStream();


    MessageStream(int sockfd);


    virtual ~MessageStream();


    ///
    /// \brief OnRecived
    /// \param p
    /// \return
    ///
    virtual int OnRecived(Epoller* p);
    virtual int OnSended(Epoller* p);

public:



    ExtClosure<void(MessageStream* sream,Epoller* p)>*  handerMessageRecived;

    ExtClosure<void(MessageStream* sream,Epoller* p)>*  handerMessageSended;


    HttpHeader reqheader;
    HttpHeader resheader;
};
}
