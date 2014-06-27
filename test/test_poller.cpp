#include "stdio.h"
#include "mio2/log.h"
#include "mio2/workpool.h"
#include "mio2/signalhelper.h"
#include "mio2/mio_timer.h"
#include "mio2/timerhelper.h"

#include "mio2/mio_poller.h"

using namespace mio2;


void hello(void *){


    char buffer[1024];
    struct timespec now;
    clock_gettime(CLOCK_REALTIME,&now);
    sprintf(buffer,"%lu %lu",now.tv_sec,now.tv_nsec);
    printf("helle %s now:%s\n ",__FUNCTION__,buffer);

    DEBUG_FMG("just say :%s ",buffer);

}


void test1(){


    IOEvent e;
    e.ev={0,0};
    e.setEvent(true,true);

    char buffer2[1024];
    SignalHelper::getBacktrace(buffer2,1024,0,5);

    std::cout<<buffer2<<std::endl;
     DEBUG_FMG("just say :%s ",buffer2);
}

int main(int argc,char*argv[]){
    DEBUG("main");
    Json::Value conf;
    LogBacker::Init(conf);

    Closure t = Closure::From(hello,NULL);


    test1();


    Epoller p;
    EventTimer et;

    p.addEvent(&et.ev, true,false);

    t();
    et.runTimerAfter(1,100,t);


    //SignalHelper::registerCallback<Epoller,&Epoller::Stop>(&p);
    p.poll();


}
