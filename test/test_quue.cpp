#include<stdio.h>
#include<iostream>
#include <sys/time.h>

#include "log/log.h"
#include "common/timerhelper.h"

#include "thread/workpool.h"
#include "common/spinlist.h"
#include "mio/mio_event.h"
#include "common/signalhelper.h"
#include "mio/mio_notify.h"

using namespace mtrpc;





#define TIMES 100000


    SpinList<char*> splist;
void test_spin(void*){



    TimeMoniter tm(0,__FUNCTION__);

    for(int i =0 ;i< TIMES;++i)
    {

        char * p = NULL;
        splist.push(p);
        splist.pop(p);
    }
}

 SpinList<int,MutexLock> mllist;
void test_mutex(void*){

    TimeMoniter tm(0,__FUNCTION__);


    for(int i =0 ;i< TIMES;++i)
    {

        mllist.push(i);

    }
    for(int i =0 ;i< TIMES;++i)
    {
        int j = 0;
        mllist.pop(j);


    }

}





void test_malloc_1024(void*){

    TimeMoniter tm(0,__FUNCTION__);

    for(int i =0 ;i< TIMES;++i)
    {
        char * p = new char[1024];
        delete [] p;
    }
}


void test_malloc_64(void*){

    TimeMoniter tm(0,__FUNCTION__);

    for(int i =0 ;i< TIMES;++i)
    {
        char * p = new char[64];
        delete [] p;
    }
}

EventNotify e;

void test_notify(void*){
    TimeMoniter tm(0,__FUNCTION__);

    for(int i =0 ;i< TIMES;++i)
    {
        e.Notify(1);
    }

}

int main(int argv,char* argc[])
{
    Json::Value conf;
    LogBacker::Init(conf);


    if(argv <= 1 )
    {
        printf("use test type\n");
        return 0;
    }
   // WorkGroup group;
   // group.Init(1);

  //  mtrpc::SignalHelper::registerCallback<mtrpc::WorkGroup,&mtrpc::WorkGroup::Stop>(&group);




    return 0;
}
