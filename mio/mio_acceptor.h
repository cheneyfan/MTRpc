#ifndef __MIO_ACCEPTOR_H_
#define __MIO_ACCEPTOR_H_

#include "mio_event.h"
#include "thread/mio_task.h"

namespace  MTRpc {

class Acceptor{
public:

    ///
    /// \brief Acceptor
    ///
    Acceptor();

    ///
    /// \brief Acceptor
    ///
    ~Acceptor();


    ///
    /// \brief onEvent
    /// \param p
    /// \param events
    ///
    void onEvent(Epoller* p,uint32_t events);

    ///
    /// \brief Listen
    /// \param host
    /// \param port
    /// \return
    ///
    int StartListen(const char* host,int port);

public:
    IOEvent ev;
    Closure onAccept;
};

}
#endif
