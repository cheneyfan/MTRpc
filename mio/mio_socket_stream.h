#ifndef _MIO_NOTIFY_H_
#define _MIO_NOTIFY_H_

#include <unistd.h>
#include <errno.h>

#include "mio_event.h"
#include "mio_buffer.h"

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
    virtual void onEvent(Epoller* p,uint32_t mask);


    virtual int onReadable(Epoller* p);
    virtual int onWriteable(Epoller* p);

    virtual int onClose(Epoller* p);

    virtual int onReadTimeOut(Epoller* p);
    virtual int onWriteimeOut(Epoller* p);


public:
    virtual int OnRecived(Epoller* p);
    virtual int OnSended(Epoller* p);

public:

    uint32_t buf_alloc_size;
    uint32_t buf_alloc_radio;

    ReadBuffer readbuf;
    WriteBuffer writebuf;
};

}
#endif
