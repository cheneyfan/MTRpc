#include "mio_event.h"
namespace mtrpc {


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
        UpdateName();
        return true;
    }

    return false;
}


void IOEvent::UpdateName(){
    UpdateName(_fd,&ev,name);
}

void IOEvent::UpdateName(int fd, epoll_event* ee, char* buf){

    buf += snprintf(buf,
                    sizeof(name),
                    "%p,%d",
                    ee->data.ptr,fd);

    int revents = ee->events;

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

    if(EPOLLRDHUP &revents)
    {
        strncpy(buf, "_RDHUP", sizeof("_RDHUP"));
        buf += sizeof("_RDHUP") - 1;
    }

    if(EPOLLONESHOT &revents)
    {
        strncpy(buf, "_ONESHOT", sizeof("_ONESHOT"));
        buf += sizeof("_ONESHOT") - 1;
    }

    if(EPOLLERR &revents)
    {
        strncpy(buf, "_ERR", sizeof("_ERR"));
        buf += sizeof("_ERR") - 1;
    }


}

}
