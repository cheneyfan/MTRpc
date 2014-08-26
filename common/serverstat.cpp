// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: <guofutan@tencent.com>
// Created: 2013-07-01
// Description:

#include "serverstat.h"
#include <iomanip>
#include "log/log.h"


#define MAX_FILE_SIZE 1024*1024*10

namespace mtrpc {
std::map<std::string, AtomicIntegerT<uint64_t> > ServerState::countermap0;
std::map<std::string, AtomicIntegerT<uint64_t> > ServerState::countermap1;
std::map<std::string, ServerState::Status> ServerState::statusmap;

static MutexLock ServerState::mutex;
volatile int ServerState::dumpinternalms = 30;
mtrpc::Worker* ServerState::worker = NULL;

void ServerState::registerCounter(const std::string& key){

    WriteLock<MutexLock> _l(mutex);


    TRACE("registerCounter:"<<key);
    countermap0[key] = AtomicIntegerT<uint64_t>();
    countermap1[key] = AtomicIntegerT<uint64_t>();
}

///
/// \brief addCounter
/// \param key
/// \param value
///
void ServerState::addCounter(const std::string& key,int value){

    std::map<std::string, AtomicIntegerT<uint64_t> >::iterator it;

    it = countermap0.find(key);

    if(it == countermap0.end())
        return;

    if( value == 1)
        it->second.increment();
    else
        it->second.add(value);
}


void ServerState::addSubCounter(const std::string& key,int value){

    std::map<std::string, AtomicIntegerT<uint64_t> >::iterator it;

    it = countermap1.find(key);

    if(it == countermap1.end())
        return;

    if( value == 1)
        it->second.increment();
    else
        it->second.add(value);
}

///
/// \brief registerStatus
/// \param key
///
void ServerState::registerStatus(const std::string& key){

     WriteLock<MutexLock> _l(mutex);

    TRACE("registerStatus:"<<key);
    statusmap[key] = Status();

    statusmap[key].stater.minvalue = 0;
    statusmap[key].stater.maxvalue = 0;
    statusmap[key].stater.avgvalue = 0;
    statusmap[key].stater.num = 0;
}

///
/// \brief setStatus
/// \param key
/// \param value
///
void ServerState::state(const std::string& key,unsigned int value){

    if( value > MAX_NUM )
    {
        WARN("state with bad value,key:"<<key<<",value:"<<value);
        return;
    }

    std::map<std::string, Status >::iterator it;

    it = statusmap.find(key);

    if(it == statusmap.end())
    {
        WARN("not register state :"<<key)
        return;
    }

    Status old = it->second;
    uint64_t *pa = &(it->second.atomic.a);
    uint64_t *pb = &(it->second.atomic.b);

    Status news;
    // state the min and max
    do{
        old = it->second;
        pa = &(it->second.atomic.a);
        news.stater.maxvalue = old.stater.maxvalue < value ? value : old.stater.maxvalue;
        news.stater.minvalue = old.stater.minvalue > value ? value : old.stater.minvalue;

    }while(!__sync_bool_compare_and_swap(pa,old.atomic.a,news.atomic.a));

    do{
        old = it->second;
        pb = &(it->second.atomic.b);

        double newavg = (double)old.stater.avgvalue * old.stater.num + value;

        news.stater.avgvalue = (newavg)/(old.stater.num + 1);
        news.stater.num = old.stater.num + 1;

    }while(!__sync_bool_compare_and_swap(pb,old.atomic.b,news.atomic.b));


}


///
/// \brief setStatus
/// \param key
/// \param value
///
void ServerState::state(const std::string& key,float value){

    if(value > MAX_NUM)
    {
        WARN("state with bad value,key:"<<key<<",value:"<<value);
        return;
    }

    std::map<std::string, Status >::iterator it;

    it = statusmap.find(key);

    if(it == statusmap.end())
    {
        WARN("not register state :"<<key)
        return;
    }

    Status old = it->second;
    uint64_t *pa = &(it->second.atomic.a);
    uint64_t *pb = &(it->second.atomic.b);


    Status news;
    // state the min and max
    do{
        old = it->second;
        pa = &(it->second.atomic.a);

        news.stater.minvalue = old.stater.minvalue > value ? value : old.stater.minvalue;

        news.stater.maxvalue = old.stater.maxvalue < value ? value : old.stater.maxvalue;

    }while(!__sync_bool_compare_and_swap(pa,old.atomic.a,news.atomic.a));

    do{
        old = it->second;
        pb = &(it->second.atomic.b);

        double newavg = (double)old.stater.avgvalue * old.stater.num + value;

        news.stater.avgvalue = (newavg)/(old.stater.num + 1);
        news.stater.num = old.stater.num + 1;

    }while(!__sync_bool_compare_and_swap(pb, old.atomic.b, news.atomic.b));

}



std::string ServerState::CounterToString(){

    std::stringstream str;
    str<<"----------------\n       {";
    std::map<std::string, AtomicIntegerT<uint64_t> >::iterator it0;
    bool isstart = false;
    for(it0 = countermap0.begin(); it0 != countermap0.end(); ++it0)
    {
        if(isstart)
        {
            isstart = false;
            str<<",\n        ";
        }

        std::map<std::string, AtomicIntegerT<uint64_t> >::iterator
                it1 =  countermap1.find(it0->first);


        if(it1 == countermap1.end())
            continue;

        if(it0->second.get() == 0
                && it1->second.get() == 0)
            continue;

        str<<"\""<<it0->first<<"\":("<<it0->second.get()<<","<<it1->second.get()<<")";
        isstart = true;
    }
    str<<"}\n";

    return str.str();
}


std::string ServerState::StatusToString(){

    std::stringstream str;
    str<<std::fixed<<std::setprecision(3)<<"----------------\n       {";

    std::map<std::string, Status>::iterator it;
    bool isstart = false;
    for(it = statusmap.begin(); it!= statusmap.end(); ++it)
    {
        if(isstart)
        {
            isstart =false;
            str<<",\n        ";
        }

        if(it->second.stater.num == 0)
            continue;

        str<<"\""<< it->first ;
        str<<"\":("<< it->second.stater.minvalue;
        str<<","<<it->second.stater.avgvalue;
        str<<","<<it->second.stater.maxvalue<<","<<it->second.stater.num<<")";

        isstart = true;
    }
    str<<"}\n";
    return str.str();
}

int ServerState::dumptofile(const std::string& name)
{
    //back up file
    struct stat statbuff;
    if(stat(name.c_str(), &statbuff) == 0 && statbuff.st_size >= MAX_FILE_SIZE){
        char cmd[1024]={0};
        char date[256]={0};
        TimerHelper::Now_Date_String(date,256);
        snprintf(cmd, sizeof(cmd),
                 "mv %s %s.%s", name.c_str(), name.c_str(),date);
        system(cmd);
    }

    FILE *fp = fopen(name.c_str(),"a+");
    if(fp == NULL){
        WARN("open "<<name<<"failed");
        return -1;
    }

    char date[256]={0};
    TimerHelper::Now_Millisecond_String(date,256);

    fprintf(fp,"%s %s %s\n",date,CounterToString().c_str(),StatusToString().c_str());
    fclose(fp);

    ServerState::Clear();
    return 0;
}

int ServerState::Clear()
{

    // clear the counter
    std::map<std::string, AtomicIntegerT<uint64_t> >::iterator it0;
    for(it0 = countermap0.begin(); it0 != countermap0.end(); ++it0) {
        it0->second.getAndSet(0);
        std::map<std::string, AtomicIntegerT<uint64_t> >::iterator it1 =  countermap1.find(it0->first);
        if(it1 != countermap1.end()) {
            it1->second.getAndSet(0);
        }
    }

    // clear the status
    std::map<std::string, Status>::iterator it;
    for(it = statusmap.begin(); it!= statusmap.end(); ++it)
    {

        Status & status = it->second;

        uint64_t *pa = &(status.atomic.a);
        uint64_t *pb = &(status.atomic.b);

        Status zeros;
        zeros.stater.avgvalue=0;
        zeros.stater.maxvalue=0;
        zeros.stater.minvalue=0;
        zeros.stater.num =0;

        uint64_t old = 0;

        do{
            old = status.atomic.a;
        }while(!__sync_bool_compare_and_swap(pa, old, zeros.atomic.a));

        do{
            old = status.atomic.b;
        }while(!__sync_bool_compare_and_swap(pb, old, zeros.atomic.b));

    }
    return 0;
}

void dumperMain(const std::string& name)
{
      TRACE("start ServerState dumpinternalms:"<<ServerState::dumpinternalms);

      while(ServerState::dumpinternalms > 0)
      {

          TRACE("Counter:"<<ServerState::CounterToString().c_str());
          TRACE("State:"<<ServerState::StatusToString().c_str());
          ServerState::Clear();
          sleep(ServerState::dumpinternalms);
      }
}

int ServerState::startDumper(const std::string& name){

     worker = new mtrpc::Worker();
     worker->RunTask(mtrpc::NewExtClosure(dumperMain,name));
     worker->start();
     return 0;
}

int ServerState::stopDumper(){
    dumpinternalms = 0;
    worker->join();
    delete worker;

    return 0;
}
}
