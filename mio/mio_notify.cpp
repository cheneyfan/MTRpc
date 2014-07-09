#include "mio_notify.h"
#include "mio_poller.h"

namespace mtrpc{

EventNotify::EventNotify():
    IOEvent(),
    handerNotify(NULL)
{
    _fd = eventfd(0, EFD_CLOEXEC|EFD_NONBLOCK);
}


EventNotify::~EventNotify(){
    ::close(_fd);
    delete handerNotify;
}



}

