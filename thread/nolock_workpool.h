// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: <guofutan@tencent.com>
// Created: 2013-07-01
// Description:

#ifndef __NOLOCK_WORK_POOL_H_
#define __NOLOCK_WORK_POOL_H_


#include <sys/syscall.h>
#include <pthread.h>

#include "common/rwlock.h"
#include "common/spinlist.h"
#include "common/singleton.h"
#include "common/timerhelper.h"
#include "workqueue.h"
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


class NoLockWorker;



class NoLockQueue
{
   public:
    NoLockQueue(uint32_t blockSize):
        task_list(new uint64_t[blockSize]),
        capacity(blockSize),
        size(0),
        cusume_id(0),
        producer_id(0)
    {
    }

    bool Put(ExtClosure<void()>* task);


    bool Get(ExtClosure<void()>*& task,NoLockWorker* w);

   public:

    uint64_t* task_list;
    int capacity;
    volatile int  size;
    volatile int cusume_id;
    volatile int producer_id;
};


class NoLockWorkGroup;
///
/// \brief The Worker struct
///
class NoLockWorker {

public:

    ///
    /// \brief Worker
    /// \param group
    ///
    NoLockWorker();

    ~NoLockWorker();
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
    static NoLockWorker* CurrentWorker();

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
    void RunTask(ExtClosure<void()>* t);
    void RunTaskOnly(ExtClosure<void()>* t);

    ///
    /// \brief Stop
    ///
    void Stop();

public:
    volatile int isruning;
    pid_t  tid;
    pthread_t threadid;


    //cache
    volatile MioTask* task;

    NoLockWorkGroup* _group;
    NoLockWorker *next;

    MutexLock mutex;
    ConditionVariable cv;

public:
    static __thread NoLockWorker *thread_worker;
    static pid_t pid;
};


///
/// \brief The WorkGroup class
///
//
class NoLockWorkGroup{
public:

    NoLockWorkGroup(unsigned int max_task);
    ~NoLockWorkGroup();

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
    void AddWork(NoLockWorker* w);
    void AddIdle(NoLockWorker* w);
    ///
    /// \brief Post
    /// \param task
    /// \return
    ///
    int Post(MioTask* task);
    bool Pop(MioTask* &task,NoLockWorker* w);

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
    NoLockWorker* workerlist;
    NoLockQueue taskqueue;
};


}

#endif
