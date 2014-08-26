#ifndef __MIO_EPOLLER_H_
#define __MIO_EPOLLER_H_



#include "common/rwlock.h"
#include "common/spinlist.h"
#include "common/ngx_rbtree.h"
#include "thread/ext_closure.h"

#define MAX_EVENT_PROCESS 100000




namespace mtrpc {

class IOEvent;
class EventNotify;

class Epoller {

public:

    Epoller();

    ~Epoller();

    ///
    /// \brief poll
    ///
    void Poll();

    ///
    /// \brief addEvent
    /// \param ev
    /// \return
    ///
    void AddEvent(IOEvent* ev, bool readable, bool wirteable);

    ///
    /// \brief ModEvent
    /// \param ev
    /// \return
    ///
    void ModEvent(IOEvent* ev,bool readable,bool wirteable);

    ///
    /// \brief DelEvent, must only run in poll thread
    /// \param ev
    /// \return
    ///
    void DelEvent(IOEvent* ev);

    ///
    /// \brief setTimeOut,must only run in poll thread
    /// \param ev
    /// \return
    ///
    void SetReadTimeOut(IOEvent* ev);


    ///
    /// \brief SetWriteTimeout,must only run in poll thread
    /// \param ev
    /// \param wsec
    /// \return
    ///
    void SetWriteTimeOut(IOEvent* ev);

    ///
    /// \brief ProcessTimeOut
    /// \return
    ///
    int ProcessTimeOut();
    ///
    /// \brief ProcessPendingTask
    ///
    void ProcessPendingTask();

    ///
    /// \brief runTask
    /// \param t
    /// \return
    ///
    void PostTask(MioTask* t);


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
    // SpinLock splock;
    /// only modify in poller thread
    ngx_rbtree_t rtimerroot;
    ngx_rbtree_t wtimerroot;
    static ngx_rbtree_node_t sentinel;

    ///
    EventNotify* _notify;
    SpinList<MioTask*,SpinLock> tasklist;

};

}

#endif
