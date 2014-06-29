
#ifndef __MIO_TCP_SERVER_H_
#define __MIO_TCP_SERVER_H_

#include "mio_acceptor.h"
#include "mio_connection.h"

#include "mio_poller.h"

#include "json/configure.h"

#include "thread/workpool.h"
namespace mtrpc {


///
/// \brief The TcpServer class
///
///
class TcpServer{

public:

    ///
    /// \brief TcpServer
    ///

    TcpServer()
    {
    }

    ~TcpServer();




    template<typename Handler>
    void AcceptConnection(int sock)
    {
         Connection<Handler>* c= new Connection<Handler>(sock);
         c->group = &group;
         poll.addEvent(&c->ev,true,false);
    }
    ///
    /// \brief Init
    /// \param conf
    /// \return

    template<typename Handler>
    int Init(const Json::Value& conf){
       onAccept = Closure::From<TcpServer,int,&TcpServer::AcceptConnection<Handler> >(this);
       host = const_cast<char*>("127.0.0.1");
       port = 1111;
       readtimeouts = 0;
       writetimeouts = 0;
       threadnum = 2;
       return 0;
    }

    ///
    /// \brief Start
    /// \return
    ///
    int Start();

    ///
    /// \brief Stop
    /// \return
    ///
    void Stop();

    ///
    /// \brief Join
    /// \return
    ///
    void Join();


public:

    Closure onAccept;



public:
    char* host;
    int port;
    uint32_t threadnum;

    uint32_t readtimeouts;
    uint32_t writetimeouts;

    Acceptor accepter;
    Epoller poll;
    WorkGroup group;

};


}
#endif
