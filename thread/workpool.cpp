// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: <guofutan@tencent.com>
// Created: 2013-07-01
// Description:

#include "log/log.h"
#include "workpool.h"

namespace mtrpc {

pid_t Worker::pid = getpid();
__thread Worker* Worker::thread_worker= NULL;


Worker::Worker():
    isruning(true),
    tid(0),
    threadid(0),
    task(NULL),
    _group(NULL),
    next(NULL),
    taskqueue(NULL),
    idleWorker(NULL)
{
}

Worker::~Worker(){
     delete task;
}


void *Worker::do_job(void* ctx){

    thread_worker = (Worker*)ctx;

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


void Worker::start(){
    pthread_create(&threadid, NULL, do_job, this);

    WriteLock<MutexLock> wl(mutex);
    //wait the thread in loop
    while(tid == 0){
        cv.wait(&mutex._mutex);
    }
}


Worker* Worker::CurrentWorker(){

    if(!thread_worker)
    {

        thread_worker = new Worker();
        thread_worker->tid = gettid();

        return thread_worker;
    }

    return thread_worker;
}

void Worker::loop()
{


    for(;isruning;)
    {
        //first job to avoid lock
        MioTask* tmp = NULL;

        { // acquire barrier : 不允许将barrier之后的内存读取指令移到barrier之前（wmb()）。
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

        if(taskqueue && taskqueue->pop(tmp))
        {
            tmp->Run();
            continue;
        }

        WriteLock<MutexLock> wl(mutex);

        if(isruning && idleWorker)
        {
            idleWorker->push(this);
            cv.wait(&mutex._mutex);
        }else{
            WARN_FMG("tid:%d Not in group exit!",tid);
            break;
        }

    }//for runing

    isruning = false;
}


void Worker::RunTask(MioTask* t)
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



void Worker::join(){

    if(threadid)
    {
        pthread_join(threadid,NULL);
    }
}

void Worker::Stop(){
    WriteLock<MutexLock> wl(mutex);
    isruning = false;
    cv.notifyOne();
}



WorkGroup::WorkGroup():
    workerlist(NULL),
    workerNum(0)
{

}

int WorkGroup::Init(uint32_t w)
{

    workerNum = ( w == 0  ? 1 : w);

    for(uint32_t i =0; i< workerNum; ++i)
    {
        Worker *w = new Worker();
        AddWork(w);
        w->start();
    }

    INFO_FMG("start workgroup with workers:%u",workerNum);

    return 0;
}


WorkGroup::~WorkGroup(){

    Worker* w = workerlist;

    while(w){
        Worker* tmp = w->next;
        delete w;
        w = tmp;
    }

}

void WorkGroup::AddWork(Worker* w)
{
    //printf("Add %p,workerlist:%p\n",w,workerlist);

    w->next = workerlist;
    workerlist = w;

    w->_group = this;
    w->idleWorker = &(this->idleWorker);
    w->taskqueue  = &(this->taskQueue);
    //printf("Add %p,workerlist:%p\n",w,workerlist);
}


int WorkGroup::Post(MioTask* task)
{
    Worker* w = NULL;
    int ret = 0;

    // if some worker is idle, wake up to do work
    if(idleWorker.pop(w))
    {
        //TRACE_FMG("Run %p with idle  worker:%p",task,w);
        w->RunTask(task);
        return 0;
    }

    // all working is doing,just push to task queue
    {
        //TRACE_FMG("Push %p to queue",task);
        ret = taskQueue.push(task);
    }


    // some work may be miss the task pushed
    while(idleWorker.pop(w))
    {
        //TRACE_FMG("notify idle worker:%p",w);
        WriteLock<MutexLock> wl(w->mutex);
        w->cv.notifyOne();
    }

    return ret;
}


Reply* WorkGroup::PostWithReply(ExtClosure<int()>* task){

       Reply* r = new Reply(task);
       Post(NewExtClosure(r,&Reply::Run));
       return r;
}


void WorkGroup::Loop(){
    this->join();
}

void WorkGroup::Stop(){

    Worker* w = workerlist;

    while(w){
        w->Stop();
        w = w->next;;
    }
}

void WorkGroup::join(){
    Worker* w = workerlist;
    while(w){
        w->join();
        w = w->next;
    }
}


}
