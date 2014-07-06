#ifndef _MIO_NOTIFY_H_
#define _MIO_NOTIFY_H_

#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <limits.h>
#include <sys/eventfd.h>

#include "mio_event.h"

namespace mtrpc{

// watch fail
class EventFile: public IOEvent {

public:



};

}
#endif
