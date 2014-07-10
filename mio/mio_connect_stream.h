#include "mio/mio_message_stream.h"

#include <google/protobuf/message.h>
#include "proto/rpc_http_header.h"
#include "thread/ext_closure.h"
#include "common/rwlock.h"

namespace mtrpc {

enum CONNECT_STATUS {
    CONNECT_FAILE = 0,
    CONNECT_Ok = 1,
    CONNECT_ING = 2,

};

typedef ::google::protobuf::Message Message;

///
/// \brief The ConnectStream class
///
class ConnectStream : public SocketStream {

public :
    ConnectStream();

    virtual ~ConnectStream();

    ///
    /// \brief Connect
    /// \param server_ip
    /// \param server_port
    /// \return
    ///
    int Connect(const std::string& server_ip,int32_t server_port);

    ///
    /// \brief OnConnect
    /// \param p
    /// \return
    ///
    virtual int OnConnect(Epoller* p);

    virtual int OnClose(Epoller* p);

    void Wait();


    ///
    /// \brief OnRecived
    /// \param p
    /// \return
    ///
    virtual int OnRecived(Epoller* p);
    virtual int OnSended(Epoller* p);


public:


    ExtClosure<void(ConnectStream* sream,Epoller* p)>*  handerMessageRecived;

    ExtClosure<void(ConnectStream* sream,Epoller* p)>*  handerMessageSended;


    volatile int _ConnectStatus;
    MutexLock mutex;
    ConditionVariable cv;


    HttpHeader reqheader;
    HttpHeader resheader;
};


}
