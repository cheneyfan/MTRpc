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
    virtual void onEvent(Epoller* p,uint32_t mask)
    {
        int ret = 0;

        if(mask & EVENT_READ)
        {
            ret |= onReadable(p);
        }

        if(mask & EVENT_WRITE)
        {
            ret |= onWriteable(p);
        }

        if(mask & EVENT_CLOSE)
        {
            ret |= -1;
        }

        if(mask & READ_TIME_OUT)
        {
            ret |= onReadTimeOut(p);
        }

        if(mask & WRITE_TIME_OUT)
        {
            ret |= onWriteimeOut(p);
        }

        if(ret < 0 )
            onClose(p);
    }


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
