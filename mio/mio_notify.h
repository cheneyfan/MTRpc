#ifndef _MTRPC__NOTIFY_H_
#define _MTRPC__NOTIFY_H_

#include <unistd.h>
#include <errno.h>
#include <iostream>

#include "mio_event.h"


namespace mtrpc{


class EventNotify: public IOEvent {

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
    virtual void OnEvent(Epoller* p,uint32_t mask){

        uint64_t counter = 0;
        int ret = ::read(_fd, (char*) &counter,sizeof(counter));
    }

    ///
    /// \brief notify_eventchannel
    /// \param fd
    /// \param signalnum
    /// \return
    ///
    int Notify(uint64_t signalnum){
        return ::write(_fd, &signalnum, sizeof(signalnum));
    }

};


}
#endif
