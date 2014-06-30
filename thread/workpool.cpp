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
    isruning(false),
    tid(0),
    threadid(0),
    _group(NULL),
    next(NULL),
    taskqueue(NULL),
    idleWorker(NULL)
{
}


void *Worker::do_job(void* ctx){

    thread_worker = (Worker*)ctx;
    thread_worker->tid = gettid();

    thread_worker->loop();

    return NULL;
}


void Worker::start(){
    pthread_create(&threadid, NULL, do_job, this);
}


Worker* Worker::CurrentWorker(){

    if(!thread_worker)
    {
        /*
        thread_worker = new Worker();
        thread_worker->tid = gettid();
        */
        return NULL;
    }

    return thread_worker;
}

void Worker::loop()
{
    isruning = true;

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
            tmp();
            continue;
        }

        if(isruning && idleWorker)
        {
            WriteLock<MutexLock> wl(mutex);
            idleWorker->push(this);
            cv.wait(&mutex._mutex);
        }else{
            //printf("tid:%d Not in group exit!\n",tid);
            break;
        }

    }//for runing
}


void Worker::RunTask(MioTask* t)
{
    // avoid the thread is moving to idleWorker list
    MioTask* tmp = __sync_lock_test_and_set(&task, t);

    if(tmp)
    {
        // this should be never happen
        WARN("worker has a undone task,repost to WorkGroup");
        _group->Post(tmp);
    }

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


int WorkGroup::Init(uint32_t workNum)
{

    workNum = workNum  ? workNum : 1;

    for(uint32_t i =0; i<workNum; ++i)
    {
        Worker *w = new Worker();
        AddWork(w);
        w->start();
    }

    return 0;
}


WorkGroup::WorkGroup():workerlist(NULL){

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
        w->RunTask(task);
        return 0;
    }

    // all working is doing,just push to task queue
    {
        ret = taskQueue.push(task);
    }


    // some work may be miss the task pushed
    if(idleWorker.pop(w))
    {
        w->cv.notifyOne();
    }

    return ret;
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
