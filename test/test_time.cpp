#include<stdio.h>
#include<iostream>
#include <sys/time.h>

#include "log/log.h"
#include "common/timerhelper.h"

using namespace mtrpc;

inline uint64_t rdtsc() {
    uint32_t low, high;
    asm volatile ("rdtsc" : "=a" (low), "=d" (high));
    return (uint64_t)high << 32 | low;
}


int fun(){
return 0;
}

void test8()
{
    TimeMoniter ttt(0,__FUNCTION__);

        unsigned long t1,t2;
        struct timespec tp;
        clock_gettime(CLOCK_REALTIME, &tp);
        tp.tv_sec +=1;

        t1 = rdtsc();

        int s  =0;
        //s= clock_nanosleep(CLOCK_REALTIME,TIMER_ABSTIME,&tp,NULL);
        usleep(1000000);

        t2 = rdtsc();

        printf("Use Time:%ld ,s:%d\n",(t2 - t1),s); //FREQUENCY  CPU的频率
}

int test_gettime(){
    TimeMoniter t1(0,__FUNCTION__);
    for(int i=0;i<10000000;++i)
        time(NULL);
    return 0;
}

int test_getimeofday(){

    TimeMoniter t1(0,__FUNCTION__);
    struct timeval tv;
    struct timezone tz;
    for(int i=0;i<10000000;++i)
        gettimeofday(&tv, &tz);
    return 0;
}

int test_getreatime(){
    TimeMoniter t1(0,__FUNCTION__);
    struct timespec tp;
    for(int i=0;i<10000000;++i)
        clock_gettime(CLOCK_REALTIME, &tp);
    return 0;

}

int test_getmonotime(){
    TimeMoniter t1(0,__FUNCTION__);

    struct timespec tp;
    for(int i=0;i<10000000;++i)
        fun();
    return 0;
}


int main(int argv,char* argc[])
{
    Json::Value conf;
    LogBacker::Init(conf);

    test8();
    test_gettime();
    test_getimeofday();
    test_getreatime();
    test_getmonotime();

    return 0;
}
