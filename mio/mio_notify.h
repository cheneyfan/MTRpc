#ifndef _MIO_NOTIFY_H_
#define _MIO_NOTIFY_H_

#include <unistd.h>
#include <errno.h>

#include "mio_event.h"
#include "mio_task.h"


namespace mio2{

class Epoller;
class EventNotify{

public:
    ///
    /// \brief new_eventchannel
    /// \return
    ///
    ///
    ///
    EventNotify();


    ~EventNotify();


public:

    ///
    /// \brief onEvent
    /// \param p
    /// \param events
    ///
    void onEvent(Epoller* p,uint32_t events);

    ///
    /// \brief notify_eventchannel
    /// \param fd
    /// \param signalnum
    /// \return
    ///
    int Notify(uint64_t signalnum){

        return ::write(ev._fd, &signalnum,sizeof(signalnum));
    }

public:
    IOEvent ev;
    Closure NotifyCallback;

};

}
#endif
