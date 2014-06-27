#include "stdio.h"
#include "iostream"
#include <sys/time.h>


#include "mio2/log.h"
#include "mio2/timerhelper.h"

#include "mio2/workpool.h"
#include "mio2/signalhelper.h"
using namespace mio2;



uint64_t i =0;
volatile int sum = 0;

void hello1(void* ctx){
    int *p = (int *)ctx;
    //sleep(1);
    char buf[20];
    sprintf(buf,"%u: do %d\n",Worker::CurrentWorker()->tid,*p);
    write(1,buf,strlen(buf));

    sum+=1;

    TRACE("log:"<<*p<<",:"<<sum);
}

int main(int argc,char* argv[]){

    Json::Value conf;
    LogBacker::Init(conf);

    WorkGroup group;

    group.Init(50);

    int vec[20000];

    for(int i=0;i<20000;i++)
    {
          vec[i] = i;
          Closure t = Closure::From(hello1,(vec+i));
          group.Post(t);
    }

    mio2::SignalHelper::registerCallback<mio2::WorkGroup,&mio2::WorkGroup::Stop>(&group);

    group.join();


    return 0;
}
