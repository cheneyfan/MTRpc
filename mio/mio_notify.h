#ifndef _MIO_NOTIFY_H_
#define _MIO_NOTIFY_H_

#include <unistd.h>
#include <errno.h>

#include "mio_event.h"
#include "thread/mio_task.h"


namespace mtrpc{


class EventNotify :public IOEvent {

public:
    ///
    /// \brief new_eventchannel
    /// \return
    ///
    ///
    ///
    EventNotify();


    virtual ~EventNotify();


public:

    ///
    /// \brief onEvent
    /// \param p
    /// \param events
    ///
    virtual void onEvent(Epoller* p,uint32_t mask);

    ///
    /// \brief notify_eventchannel
    /// \param fd
    /// \param signalnum
    /// \return
    ///
    int Notify(uint64_t signalnum){
        return ::write(_fd, &signalnum,sizeof(signalnum));
    }


};

}
#endif
