#ifndef _MTRPC__NOTIFY_H_
#define _MTRPC__NOTIFY_H_

#include <unistd.h>
#include <errno.h>
#include <iostream>

#include "mio_event.h"



#include "thread/ext_closure.h"

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

#ifdef __i386__
        while(ret>0)
             ret=::read(_fd, (char*) &counter,sizeof(counter));
#endif

        handerNotify->Run();
    }

    ///
    /// \brief notify_eventchannel
    /// \param fd
    /// \param signalnum
    /// \return
    ///
    int Notify(uint64_t signalnum){

#ifdef __x86_64__
     return ::write(_fd, &signalnum, sizeof(signalnum));
#elif __i386__
     return ::write(_fdw, &signalnum, sizeof(signalnum));
#endif


    }

public:

#ifdef __i386__
    int _fdw;
#endif

    ExtClosure<void(void)>* handerNotify;


};


}
#endif
