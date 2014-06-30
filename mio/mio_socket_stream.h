#ifndef _MIO_NOTIFY_H_
#define _MIO_NOTIFY_H_

#include <unistd.h>
#include <errno.h>

#include "mio_event.h"



namespace mtrpc{


class SocketStream :public IOEvent {

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
    /// \brief onEvent
    /// \param p
    /// \param events
    ///
    virtual void onEvent(Epoller* p,uint32_t events);

public:

    uint32_t default_buf_alloc_size;
    uint32_t default_buf_alloc_radio;
};

}
#endif
