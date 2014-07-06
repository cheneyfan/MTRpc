#include "stdio.h"
#include "log/log.h"
#include "thread/workpool.h"
#include "common/signalhelper.h"
#include "mio/mio_timer.h"
#include "common/timerhelper.h"

#include "mio/mio_poller.h"

using namespace mtrpc;


void hello(void *){


    char buffer[1024];
    struct timespec now;
    clock_gettime(CLOCK_REALTIME,&now);
    sprintf(buffer,"%lu %lu",now.tv_sec,now.tv_nsec);
    printf("helle %s now:%s\n ",__FUNCTION__,buffer);

    DEBUG_FMG("just say :%s ",buffer);

}


void test1(){



}

int main(int argc,char*argv[]){
    DEBUG("main");
    Json::Value conf;
    LogBacker::Init(conf);

   return 0;

}
