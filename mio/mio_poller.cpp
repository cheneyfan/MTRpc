#include "mio_poller.h"
#include <assert.h>

#include "log/log.h"
#include "common/rwlock.h"

namespace  MTRpc {


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
    :notify()
{
    //TODO: port to other linux
    epollfd = epoll_create1(EPOLL_CLOEXEC);

    addEvent(notify);
    sentinel.key   =0;
    sentinel.left = &sentinel;
    sentinel.right = &sentinel;

    ngx_rbtree_init(&rtimerroot, &sentinel, ngx_rbtree_insert_timer_value);
    ngx_rbtree_init(&wtimerroot, &sentinel, ngx_rbtree_insert_timer_value);
}


void Epoller::Poll()
{
    isruning = true;

    do{


        //process Timeout
        int waittime = ProcessTimeOut();

        epoll_event ev_arr[MAX_EVENT_PROCESS];

        CHECK_LOG((waittime == 0),"waittime can not be 0");

        //poll
        int nfds = epoll_wait(epollfd, ev_arr , MAX_EVENT_PROCESS, waittime);

        if(nfds < 0 )
        {
            TRACE_FMG("poll:%u return:%d, error:%d,%s",epollfd,nfds,errno,strerror(errno));
            continue;
        }

        //dispatch event
        for(int i= 0; i< nfds; ++i)
        {
            uint32_t revents = ev_arr[i].events;
            IOEvent* ev = (IOEvent*)ev_arr[i].data.ptr;

            IOEvent::updateName(ev->_fd, &(ev_arr[i]), ev->name);
            TRACE_FMG("poll:%u,event:%s",epollfd,ev->name);
            uint32_t mask = EventToMask(revents);

            ev->onEvent(this, mask);
        }


    }while(isruning);

}

int  Epoller::ProcessTimeOut(){


    time_t nowsec      =  time(NULL);
    uint32_t waittime  = nowsec;

    //process write time out
    ngx_rbtree_node_t * wleft =
            ngx_rbtree_min(wtimerroot.root, &sentinel);

    while(wleft->key > 0 && wleft->key < nowsec)
    {
        IOEvent* ioev =
                container_of(wleft,IOEvent,wtimernode);

        ioev->onEvent(this, WRITE_TIME_OUT);

        ngx_rbtree_delete(&wtimerroot, wleft);
        wleft = ngx_rbtree_min(wtimerroot.root, &sentinel);
    }

    if( wleft->key >= nowsec)
    {
        waittime = wleft->key;
    }

    // process read tinme out
    ngx_rbtree_node_t * rleft =
            ngx_rbtree_min(rtimerroot.root, &sentinel);

    while(rleft->key > 0 && rleft->key < nowsec)
    {

        IOEvent* ioev =
                container_of(rleft,IOEvent,rtimernode);
        ioev->onEvent(this, READ_TIME_OUT);

        ngx_rbtree_delete(&rtimerroot, rleft);
        rleft = ngx_rbtree_min(rtimerroot.root, &sentinel);
    }

    if(rleft->key >= nowsec &&  rleft->key < waittime)
    {
        waittime = rleft->key;
    }

    return  waittime >= nowsec  ?
                int64_t(waittime) - int64_t(nowsec) + 1:
                -1;
}


int Epoller::AddEvent(IOEvent* ev)
{

    int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, ev->_fd, &(ev->ev));
    TRACE_FMG("poll:%u,add event:%s,ret:%d",epollfd,ev->name,ret);
    return ret;
}

int Epoller::ModEvent(IOEvent* ev){

    int ret = epoll_ctl(epollfd, EPOLL_CTL_MOD, ev->_fd, &(ev->ev));

    TRACE_FMG("poll:%u,mod event:%s,ret:%d",epollfd,ev->name,ret);
    return ret;
}


int Epoller::DelEvent(IOEvent* ev){

    int ret = epoll_ctl(epollfd, EPOLL_CTL_DEL, ev->_fd, &(ev->ev));

    if(ev->wtimernode.key > 0)
    {
        ngx_rbtree_delete(&wtimerroot, &ev->wtimernode);
    }

    if(ev->rtimernode.key > 0)
    {
       ngx_rbtree_delete(&rtimerroot, &ev->rtimernode);
    }

    TRACE_FMG("poll:%u,del event:%s,timeout r:%d w:%d,ret:%d",
              epollfd,
              ev->name,
              ev->rtimernode.key,
              ev->wtimernode.key,
              ret);

    return ret;
}

int Epoller::setTimeOut(IOEvent* ev, uint32_t rsec, uint32_t wsec){
    // add to list
    // nofiy

    if(rsec){
        ev->rtimernode.key = time(NULL) + rsec;
        ev->rtimernode.left  = &sentinel;
        ev->rtimernode.right = &sentinel;

        {
            WriteLock<SpinLock> l(splock);
            ngx_rbtree_insert(&rtimerroot, &ev->rtimernode);
        }
    }


    if(wsec){
        ev->wtimernode.key = time(NULL) + wsec;
        ev->wtimernode.left  = &sentinel;
        ev->wtimernode.right = &sentinel;

        {
            WriteLock<SpinLock> l(splock);
            ngx_rbtree_insert(&wtimerroot, &ev->wtimernode);

        }
    }



    TRACE_FMG("poll:%u,set Event:%u,ctx:%p,read timeout:%u,write timeout:%u",epollfd,ev->_fd,ev->ev.data.ptr,ev->rtimernode.key,ev->wtimernode.key);
    //need update the wait time
    notify.Notify(1);

    return 0;
}

void Epoller::Stop(){
    isruning = false;
    notify.Notify(1);
}


void Epoller::RunTask(const Closure & t)
{
    tasklist.push(t);
    notify.Notify(1);
}


void Epoller::OnNotify(){

    if(tasklist.empty())
        return ;

    SpinList<Closure,SpinLock> tmp;
    tasklist.MoveTo(tmp);

    /// only at here,modify the timer root
    Closure t;
    while(!tmp.empty() && tmp.pop(t) && t.h1)
    {
        t();
    }

}

};
