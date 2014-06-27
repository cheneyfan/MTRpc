#ifndef __MIO_EPOLLER_H_
#define __MIO_EPOLLER_H_

#include "mio_event.h"
#include "mio_notify.h"
#include "rwlock.h"
#include "spinlist.h"

#define MAX_EVENT_PROCESS 1000

namespace  mio2 {

class Epoller {

public:

    Epoller();

    ///
    /// \brief poll
    ///
    void poll();

    ///
    /// \brief addEvent
    /// \param ev
    /// \return
    ///
    int addEvent(IOEvent* ev, bool isRead, bool isWrite);

    ///
    /// \brief ModEvent
    /// \param ev
    /// \return
    ///
    int  ModEvent(IOEvent* ev, bool isRead, bool isWrite);

    ///
    /// \brief DelEvent, must only run in poll thread
    /// \param ev
    /// \return
    ///
    int DelEvent(IOEvent* ev);

    ///
    /// \brief setTimeOut
    /// \param ev
    /// \return
    ///
    int setTimeOut(IOEvent* ev, uint32_t rsec, uint32_t wsec);


    ///
    /// \brief processTimeOut
    /// \return
    ///
    int processTimeOut();

    ///
    /// \brief runTask
    /// \param t
    /// \return
    ///
    void runTask(const Closure & t);

    ///
    /// \brief Stop
    ///
    void Stop();

    ///
    /// \brief HanlderNotify
    /// \param ctx
    /// \param counter
    /// \return
    ///
    void onNotify();


public:

    int epollfd;
    bool isruning;

    //
    SpinLock splock;
    ngx_rbtree_t rtimerroot;
    ngx_rbtree_t wtimerroot;

    static ngx_rbtree_node_t sentinel;

    EventNotify notify;
    SpinList<Closure,SpinLock> tasklist;

};

}

#endif
