#ifndef _MIO_CONNECTION_H_
#define _MIO_CONNECTION_H_

#include<list>
#include<string>
#include <map>

#include "mio_event.h"
#include "mio_buffer.h"


namespace mtrpc {

template<class Handler>
class Connecter
{
public:


    IOEvent ev;

public:
    Handler  handler;

    MioBufferIn input;
    MioBufferOut output;
};



}




#endif
