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

#include "common/atomic.h"
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
class WorkGroup;

/// only support the linux with epoll
class IOEvent {


public:

    /// All event use edge trigger
    IOEvent(){
      ev.events = EPOLLET;
      wtimernode.parent = NULL;
      rtimernode.parent = NULL;
      refcount = 0;
    }

    virtual ~IOEvent();

    /// the event add to epoller
    epoll_event ev;
    uint32_t _fd;

    /// pending events
    AtomicInt32 events;

    /// to debug()
    char name[32];

    /// the key
    ngx_rbtree_node_t wtimernode;
    ngx_rbtree_node_t rtimernode;

public:

    WorkGroup * group;

    AtomicInt32 refcount;

    void RequireRef(){

        refcount.incrementAndGet();
    }

    void ReleaseRef()
    {
        if(refcount.decrementAndGet() == 0)
        {
            this->OnDelete();
        }
    }

public:
    ///
    /// \brief OnEvent use virtual make the class hierarchy clearly
    /// \param p
    /// \param event_mask
    ///
    virtual void OnEvent(Epoller* p,uint32_t event_mask) = 0;



    void OnEventWrapper(Epoller* p,uint32_t event_mask){
        this->OnEvent(p,event_mask);
    }

    ///
    /// \brief OnEventAsync
    /// \param p
    ///

    virtual void  OnEventAsync(Epoller* p , uint32_t event_mask){

      ClosureP2 c = ClosureP2::From<IOEvent,Epoller*,uint32_t,&IOEvent::OnEventWrapper>(this,p,event_mask);

      if(group)
      {
          group->
      }

    }

    ///
    /// \brief OnDelete: the event will delete in pool thread
    /// \param p
    ///
    virtual OnDelete(Epoller* p) {
        delete * this;
    }

public:

     /// A wrapper to make multi-thread safe.
     /// here use epoller as input param ,so we can move a event between epollers.
    int AddEventASync(Epoller* p,bool readable,bool wirteable);

    int ModEventAsync(Epoller* p,bool readable,bool wirteable);
    int DelEventAsync(Epoller* p);
    int SetReadTimeOutAsync(Epoller* p, int time_sec);
    int SetWriteTImeoutAsync(Epoller* p,int time_sec);


public:
    //thread safe
    int SetEvent(bool readable,bool writeable);

    //on debug
    void UpdateName();
    static void UpdateName(int fd, epoll_event* ev, char* buf);

};

}
#endif
