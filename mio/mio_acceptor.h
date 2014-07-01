#ifndef __MIO_ACCEPTOR_H_
#define __MIO_ACCEPTOR_H_

#include "mio_event.h"
#include "thread/ext_closure.h"

namespace mtrpc {

class Acceptor :public IOEvent{
public:

    ///
    /// \brief Acceptor
    ///
    Acceptor();

    ///
    /// \brief Acceptor
    ///
    virtual ~Acceptor();


    ///
    /// \brief onEvent
    /// \param p
    /// \param events
    ///
    virtual void onEvent(Epoller* p,uint32_t events);

    ///
    /// \brief Listen
    /// \param host
    /// \param port
    /// \return
    ///
    int StartListen(const char* host,int port);
    int StartListen(std::string service_addres);
public:
    bool isListening;
    ExtClosure<void(int)> onAccept;
};

}
#endif
