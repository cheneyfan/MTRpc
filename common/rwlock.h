// Author: <guofutan@tencent.com>
// Created: 2013-07-01
// Description:


#ifndef _RW_LOCK_H_
#define _RW_LOCK_H_

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <string>



#include <pthread.h>

//#define barrier() __asm__ __volatile__("mfence":::"memory")
//sfence:在sfence指令前的写操作当必须在sfence指令后的写操作前完成。
//lfence：在lfence指令前的读操作当必须在lfence指令后的读操作前完成。
//mfence：在mfence指令前的读写操作当必须在mfence指令后的读写操作前完成。

///
/// \brief The SpinLock class
///
class SpinLock {
public:

    SpinLock(){
        pthread_spin_init(&_spin,0);
    }

    ~SpinLock(){
        pthread_spin_destroy(&_spin);
    }

    ///
    /// \brief uniqueLock
    ///
    void uniqueLock(){
        pthread_spin_lock(&_spin);
    }

    ///
    /// \brief unLock
    ///
    void unLock(){
        pthread_spin_unlock(&_spin);
    }

public:
    pthread_spinlock_t _spin;
};


///
/// \brief The MutexLock class
///
class MutexLock{

public:

    MutexLock()
    {
        if(0 != pthread_mutex_init(&_mutex,NULL)){
        }
    }

    ~MutexLock(){
        pthread_mutex_destroy(&_mutex);
    }

    ///
    /// \brief uniqueLock
    ///
    void uniqueLock(){
        pthread_mutex_lock(&_mutex);
    }

    ///
    /// \brief unLock
    ///
    void unLock(){
        pthread_mutex_unlock(&_mutex);
    }

public:

    pthread_mutex_t _mutex;
};


///
/// \brief The RWLock class
///
class RWLock{

public:

    RWLock()
    {
        if(0 != pthread_rwlock_init(&rwlock,NULL)){

        }
    }

    ~RWLock(){
        pthread_rwlock_destroy(&rwlock);
    }

    void shareLock(){
        pthread_rwlock_rdlock(&rwlock);
    }

    void uniqueLock(){
        pthread_rwlock_wrlock(&rwlock);
    }

    void unLock(){
        pthread_rwlock_unlock(&rwlock);
    }
public:
    pthread_rwlock_t rwlock;
};


template<class T>
class ReadLock{
public:
    ReadLock(T &rw):_rw(rw){
        _rw.shareLock();
    }

    ~ReadLock(){
        _rw.unLock();
    }

public:
    T& _rw;
};


template<class T>
class WriteLock{
public:
    WriteLock(T &rw):_rw(rw){
        _rw.uniqueLock();
    }

    ~WriteLock(){
        _rw.unLock();
    }
public:
    T& _rw;
};


class ConditionVariable {

public:
    ConditionVariable(){

        pthread_cond_init(&_cond,NULL);
    }

    ~ConditionVariable(){
        pthread_cond_destroy(&_cond);
    }

    int notifyOne(){
        return ::pthread_cond_signal(&_cond);
    }

    int  notifyAll(){
        return ::pthread_cond_broadcast(&_cond);
    }

    ///
    /// \brief wait this function may be interrupt
    /// \param mutex
    /// \return
    ///
    int wait(pthread_mutex_t * mutex){
        return ::pthread_cond_wait(&_cond,  mutex);
    }

    ///
    /// \brief wait this function may be interrupt
    /// \param mutex
    /// \param timems
    /// \return
    ///
    int wait(pthread_mutex_t * mutex,int timems)
    {
        struct timespec tp;

        clock_gettime(CLOCK_REALTIME, &tp);

        uint64_t new_nsec = tp.tv_nsec + (timems % 1000)*1000000UL;

        tp.tv_nsec = new_nsec % 1000000000UL ;
        tp.tv_sec += timems / 1000 + new_nsec/1000000000UL;

        return ::pthread_cond_timedwait(&_cond, mutex, &tp);
    }

public:
    pthread_cond_t _cond;
};

///
/// \brief The Counter class
///
class CounterDown
{
public:

    CounterDown(){}

    CounterDown(unsigned int num):
        _counter(num)
    {
    }

    void reset(unsigned int num){
        _counter = num;
    }
    ///
    /// \brief decrement
    /// \return
    ///
    int decrement()
    {
        if((--_counter) == 0 )
            cv.notifyAll();
        return 0;
    }


    void clear(){
        _counter = 0;
    }
    ///
    /// \brief wait
    /// \param timems
    /// \return
    ///
    int wait(int timems){

        if(_counter <=0 )
        {
            return 0;
        }
        
        WriteLock<MutexLock> rl(lock);
        return cv.wait(&lock._mutex, timems);
    }

public:
    MutexLock lock;
    ConditionVariable cv;
    volatile int _counter;
};


#endif
