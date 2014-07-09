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

    if(events & (EPOLLHUP|EPOLLRDHUP|EPOLLERR))
        mask |= EVENT_CLOSE;

    if(events & WRITE_TIME_OUT)
        mask |=  WRITE_TIME_OUT;

    if(events & READ_TIME_OUT)
        mask |= READ_TIME_OUT;
    return mask;
}

Epoller::Epoller()
{
    //TODO: port to other linux
    epollfd = epoll_create1(EPOLL_CLOEXEC);

    _notify = new EventNotify();
    _notify->SetEvent(true,false);
    _notify->handerNotify = NewPermanentExtClosure(this,&Epoller::ProcessPendingTask);

    AddEvent(_notify);

    sentinel.key   =0;
    sentinel.left = &sentinel;
    sentinel.right = &sentinel;

    ngx_rbtree_init(&rtimerroot, &sentinel, ngx_rbtree_insert_value);
    ngx_rbtree_init(&wtimerroot, &sentinel, ngx_rbtree_insert_value);
}

Epoller::~Epoller(){

    delete _notify;
}

void Epoller::Poll()
{
    isruning = true;

    do{
        // Pending Task may delete event or add timer

        int waittime = ProcessTimeOut();

        epoll_event ev_arr[MAX_EVENT_PROCESS];

        //TRACE("waittime:"<<waittime);

        //poll
        int nfds = epoll_wait(epollfd, ev_arr , MAX_EVENT_PROCESS, waittime);

        if(nfds <= 0 )
        {
            TRACE_FMG("poll:%u return:%d, error:%d,%s",epollfd,nfds,errno,strerror(errno));
            continue;
        }

        char event_name[64]={0};

        //dispatch event
        for(int i= 0; i< nfds; ++i)
        {
            uint32_t revents = ev_arr[i].events;
            IOEvent* ev = (IOEvent*)ev_arr[i].data.ptr;

            IOEvent::UpdateName(ev->_fd, &(ev_arr[i]), event_name);
            //TRACE_FMG("poll:%u,event:%s",epollfd,event_name);
            uint32_t mask = EventToMask(revents);
            ev->OnEventAsync(this, mask);
        }


    }while(isruning);


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

    //TRACE_FMG("timer,now:%lu,wkey:%lu,rkey:%lu,wait:%d",nowsec,wleft->key,rleft->key,waittime);

    return  waittime == uint32_t(-1)  ? -1:
                1000*(int64_t(waittime) - int64_t(nowsec) + 1);
}


void Epoller::AddEvent(IOEvent* ev)
{

    ev->ev.data.ptr = ev;
    int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, ev->_fd, &(ev->ev));
    TRACE_FMG("poll:%u,add event:%s,ret:%d",epollfd,ev->name,ret);



}

void Epoller::ModEvent(IOEvent* ev){

    int ret = epoll_ctl(epollfd, EPOLL_CTL_MOD, ev->_fd, &(ev->ev));

    TRACE_FMG("poll:%u,mod event:%s,ret:%d",epollfd,ev->name,ret);

}


void Epoller::DelEvent(IOEvent* ev){

    int ret = epoll_ctl(epollfd, EPOLL_CTL_DEL, ev->_fd, &(ev->ev));

    if(ev->wtimernode.parent)
    {
        ngx_rbtree_delete(&wtimerroot, &ev->wtimernode);
    }

    if(ev->rtimernode.parent)
    {
       ngx_rbtree_delete(&rtimerroot, &ev->rtimernode);
    }

    TRACE_FMG("poll:%u,del event:%s,timeout r:%d w:%d,ret:%d",
              epollfd,
              ev->name,
              ev->rtimernode.key,
              ev->wtimernode.key,
              ret);

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

     TRACE_FMG("poll:%u,set Event:%u,ctx:%p,read timeout:%u,write timeout:%u",epollfd,ev->_fd,ev->ev.data.ptr,ev->rtimernode.key,ev->wtimernode.key);

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

    TRACE_FMG("poll:%u,set Event:%u,ctx:%p,read timeout:%u,write timeout:%u",epollfd,ev->_fd,ev->ev.data.ptr,ev->rtimernode.key,ev->wtimernode.key);

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
