
#include "mio_socket_stream.h"


#include "proto/rpc_http_header.h"
#include "thread/ext_closure.h"
namespace mtrpc {



///
/// \brief The MessageStream class
///
class MessageStream  :public SocketStream {

public :


    MessageStream(int sockfd);


    virtual ~MessageStream();


    ///
    /// \brief OnRecived
    /// \param p
    /// \return
    ///
    virtual int OnRecived(Epoller* p,uint32_t buffer_size);
    virtual int OnSended(Epoller* p,uint32_t buffer_size);

public:



    ExtClosure<void(MessageStream* sream,Epoller* p,uint32_t buffer_size)>*  handerMessageRecived;

    ExtClosure<void(MessageStream* sream,Epoller* p,uint32_t buffer_size)>*  handerMessageSended;


    HttpRequestHeader reqheader;
    HttpReponseHeader resheader;
};
}
