#include "mio/mio_message_stream.h"
#include "proto/rpc_http_header.h"
#include "thread/ext_closure.h"
#include "common/rwlock.h"

namespace mtrpc {

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
    virtual int OnRecived(Epoller* p, int32_t buffer_size);
    virtual int OnSended(Epoller* p, int32_t buffer_size);


public:

    ExtClosure<void(ConnectStream* sream,Epoller* p,int32_t buffer_size)>*  handerMessageRecived;

    ExtClosure<void(ConnectStream* sream,Epoller* p,int32_t buffer_size)>*  handerMessageSended;


    volatile int _ConnectStatus;
    MutexLock mutex;
    ConditionVariable cv;

    HttpRequestHeader reqheader;
    HttpReponseHeader resheader;
};


}
