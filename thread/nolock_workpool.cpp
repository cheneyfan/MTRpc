// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: <guofutan@tencent.com>
// Created: 2013-07-01
// Description:

#include "log/log.h"
#include "nolock_workpool.h"

namespace mtrpc {



pid_t NoLockWorker::pid = getpid();
__thread NoLockWorker* NoLockWorker::thread_worker= NULL;


NoLockWorker::NoLockWorker():
    isruning(true),
    tid(0),
    threadid(0),
    task(NULL),
    _group(NULL),
    next(NULL)
{
}

NoLockWorker::~NoLockWorker()\
{
    delete task;
}


void *NoLockWorker::do_job(void* ctx){

    thread_worker = (NoLockWorker*)ctx;

    // notify the start method
    {
        WriteLock<MutexLock> wl(thread_worker->mutex);
        thread_worker->tid = gettid();
        thread_worker->cv.notifyOne();
    }

    INFO_FMG("worker:%p %u start loop",thread_worker,thread_worker->tid);
    thread_worker->loop();

    return NULL;
}


void NoLockWorker::start(){
    pthread_create(&threadid, NULL, do_job, this);

    WriteLock<MutexLock> wl(mutex);
    //wait the thread in loop
    while(tid == 0){
        cv.wait(&mutex._mutex);
    }
}


NoLockWorker* NoLockWorker::CurrentWorker(){

    if(!thread_worker)
    {
        thread_worker = new NoLockWorker();
        thread_worker->tid = gettid();
        return thread_worker;
    }

    return thread_worker;
}


bool NoLockQueue::Put(ExtClosure<void()>* task)
{

    int idx = __sync_fetch_and_add(&producer_id, 1) % capacity;

    uint64_t p = __sync_val_compare_and_swap(task_list+idx,0,uint64_t(task));
    if(p)
    {
        ((NoLockWorker*)p)->RunTask(task);
    }

    return true;
}

bool NoLockQueue::Get(ExtClosure<void()>*& task,NoLockWorker* w)
{

     int idx = __sync_fetch_and_add (&cusume_id, 1) % capacity;


    uint64_t p = __sync_val_compare_and_swap(task_list+idx,0,uint64_t(w));

    task = (ExtClosure<void()>*)p;

     return true;
}

void NoLockWorker::loop()
{



    for(;isruning;)
    {
        //first job to avoid lock
        MioTask* tmp = NULL;

        {
            // acquire barrier : 不允许将barrier之后的内存读取指令移到barrier之前（wmb()）。
            // release barrier : 不允许将barrier之前的内存读取指令移到barrier之后 (rmb())。
            // full barrier   : 以上两种barrier的合集(linux kernel中的mb())。
            // __sync_lock_test_and_set is acquire

            tmp =  __sync_lock_test_and_set(&task,NULL);
            if(tmp)
            {
                tmp->Run();
                continue;
            }
        }

        /// Pop is block call
        if(_group->Pop(tmp,this) && tmp)
        {
            tmp->Run();
            continue;
        }else{
            WriteLock<MutexLock> wl(mutex);
            cv.wait(&mutex._mutex);
        }
/*
        WriteLock<MutexLock> wl(mutex);

        if(isruning && _group)
        {
            _group->AddIdle(this);
            cv.wait(&mutex._mutex);
        }else{
            WARN_FMG("tid:%d Not in group exit!",tid);
            break;
        }
*/
    }//for runing

    isruning = false;
}


void NoLockWorker::RunTask(ExtClosure<void()>* t)
{
    // avoid the thread is moving to idleWorker list
    MioTask* tmp = __sync_lock_test_and_set(&task, t);

    assert(tmp == NULL);
    if(tmp)
    {
        // this should be never happen
        WARN("worker has a undone task,repost to WorkGroup");
        _group->Post(tmp);
    }

    WriteLock<MutexLock> wl(mutex);
    cv.notifyOne();
}

void NoLockWorker::RunTaskOnly(ExtClosure<void()>* t)
{
    // avoid the thread is moving to idleWorker list
    MioTask* tmp = __sync_lock_test_and_set(&task, t);

    assert(tmp == NULL);
    if(tmp)
    {
        // this should be never happen
        WARN("worker has a undone task,repost to WorkGroup");
        //_group->Post(tmp);
    }
}



void NoLockWorker::join(){

    if(threadid)
    {
        pthread_join(threadid,NULL);
    }
}

void NoLockWorker::Stop(){
    WriteLock<MutexLock> wl(mutex);
    isruning = false;
    cv.notifyOne();
}


int NoLockWorkGroup::Init(uint32_t workNum)
{

    workNum = ( workNum == 0  ? 1 : workNum);

    for(uint32_t i =0; i< workNum; ++i)
    {
        NoLockWorker *w = new NoLockWorker();
        AddWork(w);
        w->start();
        //ExtClosure<void()>* task = NULL;


        //taskqueue.Get(task,w);
        //        assert(task==NULL);
    }

    INFO_FMG("start workgroup with workers:%u",workNum);

    return 0;
}


NoLockWorkGroup::NoLockWorkGroup(unsigned int max_task):
    workerlist(NULL),
    taskqueue(max_task)
{
}

NoLockWorkGroup::~NoLockWorkGroup(){

    NoLockWorker* w = workerlist;

    while(w){
        NoLockWorker* tmp = w->next;
        delete w;
        w = tmp;
    }

}

void NoLockWorkGroup::AddWork(NoLockWorker* w)
{
    //printf("Add %p,workerlist:%p\n",w,workerlist);

    w->next = workerlist;
    workerlist = w;
    w->_group = this;


    //printf("Add %p,workerlist:%p\n",w,workerlist);
}


int NoLockWorkGroup::Post(MioTask* task)
{
    return taskqueue.Put(task) ? 0 : -1;

}


void NoLockWorkGroup::AddIdle(NoLockWorker* w)
{

}

bool NoLockWorkGroup::Pop(MioTask *&task,NoLockWorker* w)
{
    return taskqueue.Get(task,w);
}


Reply* NoLockWorkGroup::PostWithReply(ExtClosure<int()>* task)
{

    Reply* r = new Reply(task);
    Post(NewExtClosure(r,&Reply::Run));
    return r;
}

void NoLockWorkGroup::Loop()
{
    this->join();
}

void NoLockWorkGroup::Stop()
{

    NoLockWorker* w = workerlist;

    while(w)
    {
        w->Stop();
        w = w->next;;
    }
}

void NoLockWorkGroup::join()
{
    NoLockWorker* w = workerlist;
    while(w)
    {
        w->join();
        w = w->next;
    }
}


}
