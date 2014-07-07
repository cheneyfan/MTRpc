/// Mio multi-threading io framework
///
/// this is just for study linux
///

#ifndef _MTRPC_EVENT_H_
#define _MTRPC_EVENT_H_

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <stdint.h>
#include <string.h>
#include <string>

#include "common/atomic.h"
#include "common/ngx_rbtree.h"


#include "mio_error_code.h"

#ifndef offsetof
#ifdef __compiler_offsetof
    #define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
    #define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define container_of(ptr, type, member) ({ \
    const typeof( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - offsetof(type,member) );})


#endif


namespace mtrpc {

///
///
///
enum  EVENT_STATUS{

    EVENT_PENDING = 0x1,
    EVENT_PROCESSING =0x2,

    //this must avoid conflict with epoll_wait
    EVENT_READ  = 0x10,
    EVENT_WRITE = 0x20,
    EVENT_CLOSE = 0x40,
    READ_TIME_OUT = 0x80,
    WRITE_TIME_OUT = 0x100,
};




class Epoller;
class WorkGroup;

/// only support the linux with epoll
class IOEvent {


public:

    /// All event use edge trigger
    IOEvent(){
      ev.events = EPOLLET;
      wtimernode.parent = NULL;
      rtimernode.parent = NULL;
    }

    virtual ~IOEvent();

    /// the event add to epoller
    epoll_event ev;
    uint32_t _fd;

    /// pending events
    volatile uint32_t _events;

    /// to debug()
    char name[32];

    /// the key
    ngx_rbtree_node_t wtimernode;
    ngx_rbtree_node_t rtimernode;

public:

    /// if NULL OnEvent run sync,other wise run in group
    WorkGroup * group;

    AtomicInt32 refcount;

    void RequireRef()
    {
        refcount.incrementAndGet();
    }

    void ReleaseRef()
    {
        if(refcount.decrementAndGet() == 0)
        {
            delete this;
        }
    }

public:
    ///
    /// \brief OnEvent use virtual make the class hierarchy clearly
    /// \param p
    /// \param event_mask
    ///
    virtual void OnEvent(Epoller* p, uint32_t event_mask) = 0;


public:
    ///
    /// \brief OnEventWrapper forward to onEvent
    ///        because onevent is virtual
    /// \param p
    /// \param event_mask
    ///
    void OnEventWrapper(Epoller* p);

    ///
    /// \brief OnEventAsync
    /// \param p
    ///

    void  OnEventAsync(Epoller* p , uint32_t event_mask);


public:
     /// A wrapper to make multi-thread safe.
     /// here use epoller as input param ,so we can move a event between epollers.
    int AddEventASync(Epoller* p,bool readable,bool wirteable);
    int ModEventAsync(Epoller* p,bool readable,bool wirteable);
    int DelEventAsync(Epoller* p);
    int SetReadTimeOutAsync(Epoller* p, uint32_t time_sec);
    int SetWriteTimeOutAsync(Epoller* p,uint32_t time_sec);


public:
    //thread safe
    int SetEvent(bool readable,bool writeable);

    //on debug
    void UpdateName();
    static void UpdateName(int fd, epoll_event* ev, char* buf);
    static std::string EventStatusStr(uint32_t status);

};

}
#endif
