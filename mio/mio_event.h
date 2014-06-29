/// Mio multi-threading io framework
///
/// this is just for study linux
///

#ifndef _MIO2_EVENT_H_
#define _MIO2_EVENT_H_

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <stdint.h>
#include <string.h>

#include "common/ngx_rbtree.h"


#ifdef __compiler_offsetof
    #define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
    #define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif


#define container_of(ptr, type, member) ({ \
    const typeof( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - offsetof(type,member) );})


#define IOEventEntryFromTimer(left) (container_of(left,IOEvent,timernode))

#include "thread/mio_task.h"


namespace mtrpc {

///
///
///
enum  EVENT_STATUS{

    EVENT_PENDING = 0x16,
    EVENT_PROCESSING =0x32,

    //this must avoid conflict with epoll_wait
    EVENT_READ  = 1<<27,
    EVENT_WRITE = 1<<28,
    EVENT_CLOSE = 1<<29,
    READ_TIME_OUT = 1<<30,
    WRITE_TIME_OUT = 1<<31,
    EVENT_MASK = EVENT_READ|EVENT_WRITE|EVENT_CLOSE|READ_TIME_OUT|WRITE_TIME_OUT,
};




class Epoller;

/// only support the linux with epoll
class IOEvent {


public:

    /// All event use edge trigger
    IOEvent(){
      ev.events = EPOLLET;
    }

    /// the event add to epoller
    epoll_event ev;
    uint32_t _fd;

    /// pending events
    volatile uint32_t events;

    /// to debug()
    char name[32];

public:
    ///
    /// \brief OnEvent use virtual make the class hierarchy clearly
    /// \param p
    /// \param event_mask
    ///
    virtual OnEvent(Epoller* p,uint32_t event_mask);

public:
    //thread safe
    int SetEvent(bool readable,bool writeable);

    //on debug
    void UpdateName();
    static void UpdateName(int fd, epoll_event* ev, char* buf);

};

}
#endif
