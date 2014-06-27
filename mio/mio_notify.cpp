#include "mio_notify.h"
#include "mio_poller.h"

namespace mio2{

EventNotify::EventNotify(){
    ev._fd = eventfd(0, EFD_CLOEXEC|EFD_NONBLOCK);
    ev.ev.data.ptr = &ev;
    ev.onEvent = Closure::From<EventNotify, Epoller*, uint32_t, &EventNotify::onEvent>(this);
}


EventNotify::~EventNotify(){
    ::close(ev._fd);
}

void EventNotify::onEvent(Epoller* p,uint32_t events){

    uint64_t counter = 0;
    int ret = ::read(ev._fd, (char*) &counter,sizeof(counter));
    if(ret >=0)
        NotifyCallback();
}

}

