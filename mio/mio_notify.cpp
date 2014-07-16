#include "mio_notify.h"
#include "mio_poller.h"

#ifdef __x86_64__
    #include <sys/eventfd.h>
#elif __i386__
    #include <unistd.h>
#endif

namespace mtrpc{

EventNotify::EventNotify():
    IOEvent(),
    handerNotify(NULL)
{

#ifdef __x86_64__
    _fd = eventfd(0, EFD_CLOEXEC|EFD_NONBLOCK);
#elif __i386__
    int pipefd[2];
    int result = pipe(pipefd);
     _fd  = pipefd[0];//for read
     _fdw = pipefd[1];//for write
#endif
}


EventNotify::~EventNotify(){

    ::close(_fd);
#ifdef __i386__
    ::close(_fdw);
#endif

    delete handerNotify;
}



}

