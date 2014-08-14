#include "mio_poller.h"
#include <assert.h>

#include "log/log.h"
#include "common/rwlock.h"

#include "mio_notify.h"
#include "thread/ext_closure.h"
namespace mtrpc {


ngx_rbtree_node_t Epoller::sentinel;


static uint32_t EventToMask(uint32_t events)
{
    uint32_t mask = 0;
    if(events & (EPOLLIN | EPOLLPRI))
        mask |= EVENT_READ;

    if(events & EPOLLOUT)
        mask |= EVENT_WRITE;
#ifdef __x86_64__
    if(events & (EPOLLHUP|EPOLLRDHUP))
        mask |= EVENT_CLOSE;
#elif __i386__
    if(events & (EPOLLHUP))
        mask |= EVENT_CLOSE;
#endif

    if(events & WRITE_TIME_OUT)
        mask |=  WRITE_TIME_OUT;

    if(events & READ_TIME_OUT)
        mask |= READ_TIME_OUT;

    if(events & (EPOLLERR))
        mask |= EVENT_ERR;
    return mask;
}

Epoller::Epoller()
{
    //TODO: port to other linux
#ifdef __x86_64__
    epollfd = epoll_create1(EPOLL_CLOEXEC);
#else
    epollfd = epoll_create(1024);
#endif

    _notify = new EventNotify();
    _notify->SetEvent(true,false);
    _notify->handerNotify = NewPermanentExtClosure(this,&Epoller::ProcessPendingTask);

    _notify->RequireRef();
    AddEvent(_notify);

    sentinel.key   =0;
    sentinel.left = &sentinel;
    sentinel.right = &sentinel;

    ngx_rbtree_init(&rtimerroot, &sentinel, ngx_rbtree_insert_value);
    ngx_rbtree_init(&wtimerroot, &sentinel, ngx_rbtree_insert_value);
}

Epoller::~Epoller(){

    DelEvent(_notify);
    delete _notify;
    close(epollfd);
}

void Epoller::Poll()
{
    isruning = true;

    epoll_event* ev_arr = new epoll_event[MAX_EVENT_PROCESS];

    do{
        // Pending Task may delete event or add timer

        int waittime = ProcessTimeOut();



        //TRACE("waittime:"<<waittime);
        int nfds = epoll_wait(epollfd, ev_arr , MAX_EVENT_PROCESS, waittime);

        if(nfds <= 0 )
        {
            TRACE_FMG("poll:%u return:%d, error:%d,%s",epollfd,nfds,errno,strerror(errno));
            continue;
        }

        //dispatch event
        for(int i= 0; i< nfds; ++i)
        {
            uint32_t revents = ev_arr[i].events;
            IOEvent* ev = (IOEvent*)ev_arr[i].data.ptr;
            if(ev->_fd  == _notify->_fd)
                continue;
            TRACE("Poll:"<<epollfd<<",event:"<<ev->GetEventName());
            uint32_t mask = EventToMask(revents);
            ev->OnEventAsync(this, mask);
        }

        ProcessPendingTask();


    }while(isruning);

    delete [] ev_arr;
}

int  Epoller::ProcessTimeOut(){


    time_t nowsec      =  time(NULL);
    uint32_t waittime  = -1;

    //process write time out
    ngx_rbtree_node_t * wleft =
            ngx_rbtree_min(wtimerroot.root, &sentinel);

    while(wleft->key > 0 && wleft->key <= nowsec)
    {
        IOEvent* ioev = static_cast<IOEvent*>(wleft->data);

        ioev->OnEventAsync(this, WRITE_TIME_OUT);
        ioev->ReleaseRef();

        ngx_rbtree_delete(&wtimerroot, wleft);
        wleft = ngx_rbtree_min(wtimerroot.root, &sentinel);
    }

    if( wleft->key > nowsec)
    {
        waittime = wleft->key;
    }

    // process read tinme out
    ngx_rbtree_node_t * rleft =
            ngx_rbtree_min(rtimerroot.root, &sentinel);

    while(rleft->key > 0 && rleft->key <= nowsec)
    {

        IOEvent* ioev = static_cast<IOEvent*>(rleft->data);

        ioev->OnEventAsync(this, READ_TIME_OUT);
        ioev->ReleaseRef();

        ngx_rbtree_delete(&rtimerroot, rleft);
        rleft = ngx_rbtree_min(rtimerroot.root, &sentinel);
    }

    if(rleft->key != 0)
    {
        waittime = rleft->key < waittime ? rleft->key : waittime ;
    }

    return  waittime == uint32_t(-1)  ? -1:
                1000*(int64_t(waittime) - int64_t(nowsec) + 1);
}


void Epoller::AddEvent(IOEvent* ev)
{

    ev->ev.data.ptr = ev;
    int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, ev->_fd, &(ev->ev));
    TRACE("Poll:"<<epollfd<<",add event:"<<ev->GetEventName()<<",ret:"<<ret);
}

void Epoller::ModEvent(IOEvent* ev){

    int ret = epoll_ctl(epollfd, EPOLL_CTL_MOD, ev->_fd, &(ev->ev));
    TRACE("Poll:"<<epollfd<<",mod event:"<<ev->GetEventName()<<",ret:"<<ret);

}


void Epoller::DelEvent(IOEvent* ev){

    int ret = epoll_ctl(epollfd, EPOLL_CTL_DEL, ev->_fd, NULL);

    if(ev->wtimernode.parent)
    {
        ngx_rbtree_delete(&wtimerroot, &ev->wtimernode);
    }

    if(ev->rtimernode.parent)
    {
       ngx_rbtree_delete(&rtimerroot, &ev->rtimernode);
    }

    TRACE("Poll:"<<epollfd<<",del event:"<<ev->GetEventName()<<",ret:"<<ret);

    ev->ReleaseRef();
}

void Epoller::SetReadTimeOut(IOEvent* ev){

    // if exists delete
    if(ev->rtimernode.parent)
    {
        ngx_rbtree_delete(&rtimerroot, &ev->rtimernode);
    }
    else{

        ev->rtimernode.parent = NULL;
        ev->rtimernode.left  = &sentinel;
        ev->rtimernode.right = &sentinel;
        ev->RequireRef();
    }

     ngx_rbtree_insert(&rtimerroot, &ev->rtimernode);
     TRACE("Poll:"<<epollfd<<",event:"<<ev->GetEventName()<<",set readout:"<<ev->rtimernode.key);
     _notify->Notify(1);

}

void Epoller::SetWriteTimeOut(IOEvent* ev){

    // if exists delete
    if(ev->wtimernode.parent)
    {
        ngx_rbtree_delete(&wtimerroot, &ev->wtimernode);
    }
    else{
        ev->wtimernode.parent = NULL;
        ev->wtimernode.left  = &sentinel;
        ev->wtimernode.right = &sentinel;
        ev->RequireRef();
    }

     ngx_rbtree_insert(&wtimerroot, &ev->wtimernode);

    //need update the wait time
    TRACE("Poll:"<<epollfd<<",event:"<<ev->GetEventName()<<",set writeout:"<<ev->wtimernode.key);

    _notify->Notify(1);

}

void Epoller::Stop(){
    isruning = false;
    _notify->Notify(1);
}


void Epoller::PostTask(MioTask *t)
{
    tasklist.push(t);
    _notify->Notify(1);
}


void Epoller::ProcessPendingTask(){

    if(tasklist.empty())
        return ;

    SpinList<MioTask*,SpinLock> tmp;
    tasklist.MoveTo(tmp);

    /// only at here,modify the timer root
    MioTask* t = NULL;
    while(!tmp.empty() && tmp.pop(t) && t)
    {
        t->Run();
    }

}

};
