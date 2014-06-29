#ifndef __MIO_EPOLLER_H_
#define __MIO_EPOLLER_H_

#include "mio_event.h"
#include "mio_notify.h"

#include "common/rwlock.h"
#include "common/spinlist.h"

#define MAX_EVENT_PROCESS 1000

namespace  MTRpc {

class Epoller {

public:

    Epoller();

    ///
    /// \brief poll
    ///
    void Poll();

    ///
    /// \brief addEvent
    /// \param ev
    /// \return
    ///
    int AddEvent(IOEvent* ev);

    ///
    /// \brief ModEvent
    /// \param ev
    /// \return
    ///
    int  ModEvent(IOEvent* ev);

    ///
    /// \brief DelEvent, must only run in poll thread
    /// \param ev
    /// \return
    ///
    int DelEvent(IOEvent* ev);

    ///
    /// \brief setTimeOut,must only run in poll thread
    /// \param ev
    /// \return
    ///
    int SetReadTimeOut(IOEvent* ev, uint32_t rsec);


    ///
    /// \brief SetWriteTimeout,must only run in poll thread
    /// \param ev
    /// \param wsec
    /// \return
    ///
    int SetWriteTimeout(IOEvent* ev, uint32_t wsec);

    ///
    /// \brief ProcessTimeOut
    /// \return
    ///
    int ProcessTimeOut();

    ///
    /// \brief runTask
    /// \param t
    /// \return
    ///
    void PostTask(const Closure & t);

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
    void OnNotify();


public:

    int epollfd;
    volatile bool isruning;

    //
    //SpinLock splock;
    /// only modify in poller thread
    ngx_rbtree_t rtimerroot;
    ngx_rbtree_t wtimerroot;
    static ngx_rbtree_node_t sentinel;

    ///
    EventNotify notify;
    SpinList<Closure,SpinLock> tasklist;

};

}

#endif
