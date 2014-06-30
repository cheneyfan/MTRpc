#include "mio_notify.h"
#include "mio_poller.h"

namespace mtrpc{

EventNotify::EventNotify(){

    _fd = eventfd(0, EFD_CLOEXEC|EFD_NONBLOCK);
}


EventNotify::~EventNotify(){
    ::close(ev._fd);
}

void EventNotify::onEvent(Epoller* p,uint32_t events){

    uint64_t counter = 0;
    int ret = ::read(ev._fd, (char*) &counter,sizeof(counter));
}

}

