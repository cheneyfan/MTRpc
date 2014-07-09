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
    virtual int onReadable(Epoller* p);
    virtual int onWriteable(Epoller* p);

    virtual int onClose(Epoller* p);

    virtual int onReadTimeOut(Epoller* p);
    virtual int onWriteimeOut(Epoller* p);

public:
    //forward the event
    ExtClosure<void(SocketStream* sream,Epoller* p)>* handerConnected;
    ExtClosure<void(SocketStream* sream,Epoller* p)>* handerReadable;
    ExtClosure<void(SocketStream* sream,Epoller* p)>* handerWriteable;
    ExtClosure<void(SocketStream* sream,Epoller* p)>* handerClose;
    ExtClosure<void(SocketStream* sream,Epoller* p)>* handerReadTimeOut;
    ExtClosure<void(SocketStream* sream,Epoller* p)>* handerWriteimeOut;

public:
    virtual int OnRecived(Epoller* p);
    virtual int OnSended(Epoller* p);




public:
    bool _isConnected;
    std::string peer_ip;
    int peer_port;
    std::string local_ip;
    int local_port;

    uint32_t buf_alloc_size;

    ReadBuffer readbuf;
    WriteBuffer writebuf;
    WriteBuffer::Iterator packetEnd;


};

}
#endif
