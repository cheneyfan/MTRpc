#include "mio_socket_stream.h"

#include <google/protobuf/message.h>
#include "proto/rpc_http_header.h"
#include "thread/ext_closure.h"


namespace mtrpc {

enum CONNECT_STATUS {
    CONNECT_FAILE = 0,
    CONNECT_Ok = 1,
    CONNECT_ING = 2,

};

typedef ::google::protobuf::Message Message;

class ConnectStream : public SocketStream {

public :
    ConnectStream();

    ///
    /// \brief Connect
    /// \param server_ip
    /// \param server_port
    /// \return
    ///
    int Connect(const std::string& server_ip,int32_t server_port);

    virtual int OnConnect(Epoller* p);
public:

    int _ConnectStatus;
    ExtClosure<void(ConnectStream* sream, Epoller* p)>*  handerConnected;
    ExtClosure<void(ConnectStream* sream, Epoller* p)>*  handerMessageRecived;

    ExtClosure<void(ConnectStream* sream, Epoller* p)>*  handerMessageSended;

public:
    HttpHeader reqheader;
    HttpHeader resheader;

};
}
