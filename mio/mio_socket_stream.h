#ifndef _MIO_NOTIFY_H_
#define _MIO_NOTIFY_H_

#include <unistd.h>
#include <errno.h>

#include "mio_event.h"
#include "mio_buffer.h"
#include "thread/ext_closure.h"
namespace mtrpc{


class SocketStream : public IOEvent {

public:
    ///
    /// \brief new_eventchannel
    /// \return
    ///
    ///
    ///
    SocketStream();


    virtual ~SocketStream();


public:

    ///
    /// \brief onEvent dispath the event to io event
    /// \param p
    /// \param events
    ///
    virtual void OnEvent(Epoller* p,uint32_t mask);


    virtual int OnConnect(Epoller* p);
    virtual int OnReadable(Epoller* p);
    virtual int OnWriteable(Epoller* p);

    virtual int OnClose(Epoller* p);

    virtual int OnReadTimeOut(Epoller* p);
    virtual int OnWriteimeOut(Epoller* p);


public:
    //forward the event
    ExtClosure<void(SocketStream* sream,Epoller* p)>* handerConnected;
    ExtClosure<void(SocketStream* sream,Epoller* p)>* handerReadable;
    ExtClosure<void(SocketStream* sream,Epoller* p)>* handerWriteable;
    ExtClosure<void(SocketStream* sream,Epoller* p)>* handerClose;
    ExtClosure<void(SocketStream* sream,Epoller* p)>* handerReadTimeOut;
    ExtClosure<void(SocketStream* sream,Epoller* p)>* handerWriteimeOut;
    ExtClosure<void(SocketStream* sream,Epoller* p,uint32_t error_code)>* handerMessageError;

public:

    //
    virtual int OnRecived(Epoller* p,uint32_t buffer_size);
    virtual int OnSended(Epoller* p,uint32_t buffer_size);


    std::string GetSockName();

public:
    volatile bool _isConnected;
    volatile bool _close_when_empty;

    std::string peer_ip;
    int peer_port;
    std::string local_ip;
    int local_port;

    ReadBuffer readbuf;
    WriteBuffer writebuf;
};

}
#endif
