#include <stdio.h>
#include <iostream>
#include <sys/time.h>


#include "log/log.h"

#include "thread/nolock_workpool.h"
#include "common/signalhelper.h"
#include "common/timerhelper.h"

using namespace mtrpc;



uint64_t i =0;
volatile int sum = 0;


struct Delay {

public:
    int id;
    int tid;
    uint64_t push;
    uint64_t start;
    uint64_t done;
    uint64_t finish;
    
    std::string toString(){
        
        char buf[256]={0};
        uint64_t waitque = start -push;
        uint64_t dotime  =  done - start;
        uint64_t waitfinish = finish - done;
        
        snprintf(buf,256,"id:%d,tid:%d,waitinqueue:%lu,do:%lu,waitfinish:%lu", id,tid, waitque, dotime, waitfinish);
        return std::string(buf);
    }
};

int testDelay(Delay * d)
{
    d->start = TimerHelper::Now_Microsecond();

    d->tid   = NoLockWorker::CurrentWorker()->tid;
    usleep(1000);
    //TRACE("do:"<<d->id);
    d->done  = TimerHelper::Now_Microsecond(); 
}

int main(int argc,char* argv[]){

    std::cout.sync_with_stdio(false);
    //Json::Value conf;
    //LogBacker::Init(conf);
    
    int threadnum = 1;
    
    if(argc > 1)
        threadnum = atoi(argv[1]);
    
    int testnum = 10;
    if(argc  > 2)
        testnum = atoi(argv[2]);

    NoLockWorkGroup group(1024*1024);
    
    group.Init(threadnum);
    
    std::vector<Delay> delay;
    std::vector<Reply*> res;
    
    delay.resize(testnum);
    res.resize(testnum);
    
    uint64_t start = TimerHelper::Now_Microsecond();
    for(int i = 0; i < testnum; i++)
    {
        delay[i].id = i;
        Delay* d  = &(delay[i]);
        d->push = TimerHelper::Now_Microsecond();
        res[i] = group.PostWithReply( NewExtClosure( testDelay, d));
    }
    
    for(int i = 0; i < testnum; i++)
    {
        res[i]->Wait();
        delay[i].finish = TimerHelper::Now_Microsecond();
        TRACE(delay[i].toString());
    }
    
    TRACE("all take:"<<TimerHelper::Now_Microsecond() - start);
    group.join();


    return 0;
}
