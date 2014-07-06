#include "mio_notify.h"
#include "mio_poller.h"

namespace mtrpc{

EventNotify::EventNotify(){

    _fd = eventfd(0, EFD_CLOEXEC|EFD_NONBLOCK);
}


EventNotify::~EventNotify(){
    ::close(_fd);
}

void EventNotify::onEvent(Epoller* p,uint32_t mask){

    uint64_t counter = 0;
    int ret = ::read(_fd, (char*) &counter,sizeof(counter));
}

}

