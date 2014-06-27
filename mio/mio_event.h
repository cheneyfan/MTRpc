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

#include "ngx_rbtree.h"


#ifdef __compiler_offsetof
    #define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
    #define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif


#define container_of(ptr, type, member) ({ \
    const typeof( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - offsetof(type,member) );})


#define IOEventEntryFromTimer(left) (container_of(left,IOEvent,timernode))

#include "mio_task.h"


namespace mio2 {

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
    ngx_rbtree_node_t rtimernode;
    ngx_rbtree_node_t wtimernode;

public:
    IOEvent(){

      rtimernode.key = 0;
      wtimernode.key = 0;
      ev.events = EPOLLET;
    }

    epoll_event ev;

    uint32_t _fd;

    /** pending events*/
    volatile uint32_t events;


public:
    Closure onEvent;
public:
    //thread safe
    int setEvent(bool readable,bool writeable);

    //on debug
    void updateName();
    static void updateName(int fd, epoll_event* ev, char* buf);
    char name[32];
};

}
#endif
