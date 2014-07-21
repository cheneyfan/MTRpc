#include "mio_event.h"
#include "log/log.h"
#include "mio_poller.h"

#include "thread/ext_closure.h"
#include "thread/workpool.h"

namespace mtrpc {


IOEvent::IOEvent()
{
    ev.events = EPOLLET;
    ev.data.ptr = this;

    _events = 0;
    _fd =0;

    wtimernode.parent = NULL;
    rtimernode.parent = NULL;

    group = NULL;
}

IOEvent::~IOEvent(){
    TRACE("event:"<<GetEventName()<<" dead");

}

int IOEvent::SetEvent(bool readable,bool writeable){

    uint32_t old = ev.events;

    ev.events = EPOLLET;

    if(readable)
        ev.events |= (EPOLLIN|EPOLLPRI);
    else
        ev.events &= ~(EPOLLIN|EPOLLPRI);

    if(writeable)
        ev.events |= EPOLLOUT;
    else
        ev.events &= ~EPOLLOUT;

    if(old != ev.events)
    {

        return true;
    }

    return false;
}






void IOEvent::OnEventWrapper(Epoller* p){
    do{
        //set processing
        int pre_mask =
                __sync_lock_test_and_set(&_events, EVENT_PROCESSING);

        assert(pre_mask&EVENT_PENDING);

        TRACE(GetStatusName()<<",pre_mask:"<<StatusToStr(pre_mask));

        this->OnEvent(p, pre_mask);

        // if not set events when process, set events to 0
    }while(!__sync_bool_compare_and_swap (&_events, EVENT_PROCESSING,0) );

    ReleaseRef();
}

///
/// \brief OnEventAsync
/// \param p
///

void IOEvent::OnEventAsync(Epoller* p , uint32_t event_mask){

    if(!group)
    {
        this->OnEvent(p, event_mask);
        return;
    }


    uint32_t pre_mask =
            __sync_fetch_and_or(&_events, (event_mask | EVENT_PENDING));

    /*TRACE_FMG("name:%s, event_mask:%s, pre_mask:%s, _events:%s",
              name,
              EventStatusStr(event_mask).c_str(),
              EventStatusStr(pre_mask).c_str(),
              EventStatusStr(_events).c_str());
              */

    if(pre_mask & EVENT_PENDING )
    {
        return ;
    }

    if(pre_mask & EVENT_PROCESSING )
    {
        return ;
    }

    RequireRef();
    ///
    MioTask * closure =
            NewExtClosure(this,&IOEvent::OnEventWrapper,p);
    group->Post(closure);
}



int IOEvent::AddEventASync(Epoller* p,bool readable,bool wirteable)
{
    //ev.data.ptr = this;
    this->SetEvent(readable,wirteable);

    RequireRef();

    MioTask * closure =
            NewExtClosure(p,&Epoller::AddEvent,this);

    p->PostTask(closure);

    return 0;
}

int IOEvent::ModEventAsync(Epoller* p,bool readable,bool wirteable)
{
    if(this->SetEvent(readable,wirteable))
    {

        MioTask * closure =
                NewExtClosure(p,&Epoller::ModEvent,this);

        p->PostTask(closure);
    }

    return 0;
}

int IOEvent::DelEventAsync(Epoller* p)
{
    MioTask * closure =
            NewExtClosure(p,&Epoller::DelEvent,this);

    p->PostTask(closure);
    return 0;
}


int IOEvent::SetReadTimeOutAsync(Epoller* p,uint32_t time_sec)
{

    rtimernode.data = this;
    rtimernode.key = time(NULL) + time_sec;
    MioTask * closure =
            NewExtClosure(p,&Epoller::SetReadTimeOut,this);

    p->PostTask(closure);

    return 0;

}

int IOEvent::SetWriteTimeOutAsync(Epoller* p,uint32_t time_sec)
{

    wtimernode.data = this;
    wtimernode.key = time(NULL) + time_sec;

    MioTask * closure =
            NewExtClosure(p,&Epoller::SetWriteTimeOut,this);

    p->PostTask(closure);

    return 0;
}


std::string IOEvent::EventToStr(uint32_t revents){

    char buffer[64]={0};
    char *buf = buffer;


    if(EPOLLET &revents)
    {
        strncpy(buf, "_ET", sizeof("_ET"));
        buf += sizeof("_ET") - 1;
    }

    if(EPOLLIN &revents)
    {
        strncpy(buf, "_IN", sizeof("_IN"));
        buf += sizeof("_IN") - 1;
    }

    if(EPOLLPRI &revents)
    {
        strncpy(buf, "_PRI", sizeof("_PRI"));
        buf += sizeof("_PRI") - 1;
    }

    if(EPOLLOUT &revents)
    {
        strncpy(buf, "_OUT", sizeof("_OUT"));
        buf += sizeof("_OUT") - 1;
    }

    if(EPOLLRDNORM &revents)
    {
        strncpy(buf, "_RDNORM", sizeof("_RDNORM"));
        buf += sizeof("_RDNORM") - 1;
    }

    if(EPOLLRDBAND &revents)
    {
        strncpy(buf, "_RDBAND", sizeof("_RDBAND"));
        buf += sizeof("_RDBAND") - 1;
    }

    if(EPOLLWRNORM &revents)
    {
        strncpy(buf, "_WRNORM", sizeof("_WRNORM"));
        buf += sizeof("_WRNORM") - 1;
    }

    if(EPOLLWRBAND &revents)
    {
        strncpy(buf, "_WRBAND", sizeof("_WRBAND"));
        buf += sizeof("_WRBAND") - 1;
    }

    if(EPOLLMSG &revents)
    {
        strncpy(buf, "_MSG", sizeof("_MSG"));
        buf += sizeof("_MSG") - 1;
    }

    if(EPOLLERR &revents)
    {
        strncpy(buf, "_ERR", sizeof("_ERR"));
        buf += sizeof("_ERR") - 1;
    }

    if(EPOLLHUP &revents)
    {
        strncpy(buf, "_HUP", sizeof("_HUP"));
        buf += sizeof("_HUP") - 1;
    }

#ifdef __x86_64__
    if(EPOLLRDHUP &revents)
    {
        strncpy(buf, "_RDHUP", sizeof("_RDHUP"));
        buf += sizeof("_RDHUP") - 1;
    }
#endif
    
    if(EPOLLONESHOT &revents)
    {
        strncpy(buf, "_ONESHOT", sizeof("_ONESHOT"));
        buf += sizeof("_ONESHOT") - 1;
    }

    return std::string(buffer);
}

std::string IOEvent::StatusToStr(uint32_t status){

    char buf[256]={0};
    char* ptr = buf;

    if(status & EVENT_PENDING)
    {
        strncpy(ptr,"PEND|",sizeof("PEND|"));
        ptr+=sizeof("PEND|") -1;
    }

    if(status&EVENT_PROCESSING)
    {
        strncpy(ptr,"PROC|",sizeof("PROC|"));
        ptr+=sizeof("PROC|")-1;
    }

    if(status&EVENT_READ)
    {
        strncpy(ptr,"RD|",sizeof("RD|"));
        ptr+=sizeof("RD|")-1;
    }

    if(status&EVENT_WRITE)
    {
        strncpy(ptr,"WR|",sizeof("WR|"));
        ptr+=sizeof("WR|")-1;
    }

    if(status&EVENT_CLOSE)
    {
        strncpy(ptr,"CLOSE|",sizeof("CLOSE|"));
        ptr+=sizeof("CLOSE|")-1;
    }

    if(status&READ_TIME_OUT)
    {
        strncpy(ptr,"RDOUT|",sizeof("RDOUT|"));
        ptr+=sizeof("RDOUT|")-1;
    }

    if(status&WRITE_TIME_OUT)
    {
        strncpy(ptr,"WROUT|",sizeof("WROUT|"));
        ptr+=sizeof("WROUT|")-1;
    }


    if(status&EVENT_ERR)
    {
        strncpy(ptr,"ERR|",sizeof("ERR|"));
        ptr+=sizeof("ERR|")-1;
    }

    return std::string(buf);

}

std::string IOEvent::GetEventName(){
     char name[64]={0};
     snprintf(name,sizeof(name),"%d%s",_fd,EventToStr(ev.events).c_str());
     return std::string(name);
}

std::string IOEvent::GetStatusName(){
    char name[64]={0};
    snprintf(name,sizeof(name),"%d_%s",_fd,StatusToStr(_events).c_str());
    return std::string(name);
}

}
