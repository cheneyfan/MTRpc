// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: <guofutan@tencent.com>
// Created: 2013-07-01
// Description:

#ifndef __WORK_POOL_H_
#define __WORK_POOL_H_


#include <sys/syscall.h>
#include <pthread.h>

#include "common/rwlock.h"
#include "common/spinlist.h"
#include "common/singleton.h"
//#define ENABLE_WORK_POOL_LOG

#define gettid() syscall(SYS_gettid)

#include "ext_closure.h"

namespace mtrpc{


class Reply
{
public:

     Reply(ExtClosure<int()>* task):
         ret(0),
         isdone(false),
         _task(task)
     {
     }

     ~Reply(){}

     void Run()
     {
         ret = _task->Run();
         {
            WriteLock<MutexLock> _lc(_mutex);
            isdone = true;
            _cv.notifyOne();
         }
     }

     int  Wait()
     {
         while(!isdone)
         {
             WriteLock<MutexLock> _lc(_mutex);
             _cv.wait(&_mutex._mutex);
         }
         
         return ret;
     }

private:
    volatile int ret;
    volatile bool isdone;
    ExtClosure<int()>* _task;
    MutexLock _mutex;
    ConditionVariable _cv;
    
};


class WorkGroup;
///
/// \brief The Worker struct
///
class Worker {

public:

    ///
    /// \brief Worker
    /// \param group
    ///
    Worker();

    ~Worker();
    ///
    /// \brief do_job
    /// \param ctx
    ///
    static void* do_job(void* ctx);

    ///
    /// \brief start
    ///
    void start();

    ///
    /// \brief CurrentWorker
    /// \return
    ///
    static Worker* CurrentWorker();

    ///
    /// \brief loop
    ///
    void loop();

    ///
    /// \brief join
    ///
    void join();

    ///
    /// \brief RunTask
    /// \param t
    ///
    void RunTask(MioTask* t);

    ///
    /// \brief Stop
    ///
    void Stop();

public:
    volatile int isruning;
    pid_t  tid;
    pthread_t threadid;


    SpinLock spin;
    MioTask* task;

    WorkGroup* _group;
    Worker *next;

    SpinList<MioTask*,MutexLock>* taskqueue;
    SpinList<Worker*,MutexLock>* idleWorker;


    MutexLock mutex;
    ConditionVariable cv;

public:
    static __thread Worker *thread_worker;
    static pid_t pid;


};


///
/// \brief The WorkGroup class
///
class WorkGroup{
public:

    WorkGroup();
    ~WorkGroup();

    ///
    /// \brief Init
    /// \param workNum
    /// \return
    ///
    int Init(uint32_t workNum);

    ///
    /// \brief AddWork
    /// \param w
    ///
    void AddWork(Worker* w);

    ///
    /// \brief Post
    /// \param task
    /// \return
    ///
    int Post(MioTask* task);


    Reply* PostWithReply(ExtClosure<int()>* task);

    ///
    /// \brief Loop
    ///
    void Loop();

    ///
    /// \brief Stop
    ///
    void Stop();

    ///
    /// \brief join
    ///
    void join();

public:
    Worker* workerlist;
    SpinList<MioTask*,MutexLock> taskQueue;
    SpinList<Worker* ,MutexLock> idleWorker;

};


}

#endif
