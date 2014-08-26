// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: <guofutan@tencent.com>
// Created: 2013-07-01
// Description:
#ifndef _SERVER_STAT_H_
#define _SERVER_STAT_H_

#include <map>
#include <sstream>
#include <limits.h>
#include "atomic.h"

#include "thread/workpool.h"
#include "timerhelper.h"
#define MAX_NUM 200000000

namespace mtrpc {
class ServerState
{
public:
    static void registerCounter(const std::string& key);
    static void addCounter(const std::string& key,int value = 1);
    static void addSubCounter(const std::string& key,int value = 1);


    static void registerStatus(const std::string& key);

    static void state(const std::string& key,unsigned value);

    static void state(const std::string& key,float value);
    static void state(const std::string& key,double value){
        state(key,(float)value);
    }
    static void state(const std::string& key,int64_t value){
        state(key,(unsigned)value);
    }
    static void state(const std::string& key,uint64_t value){
        state(key,(unsigned)value);
    }
    static void state(const std::string& key,int value){
         state(key,(unsigned)value);
    }

    static std::string CounterToString();
    static std::string StatusToString();
    static int dumptofile(const std::string& name);
    static int Clear();
    static int startDumper(const std::string& name);
    static int stopDumper();

public:
    class StateTimeMs
    {
    public:
        explicit StateTimeMs(std::string key, bool autoState = true)
            : m_key(key)
            , m_startms(TimerHelper::Now_Millisecond())
            , m_autoState(autoState)

        {
        }
        void Reset(const std::string& key)
        {
            m_key = key;
            m_startms = TimerHelper::Now_Millisecond();
        }
        uint64_t Elapse()
        {
            uint64_t elapse = TimerHelper::Now_Millisecond() - m_startms;
            ServerState::state(m_key, elapse);
            return elapse;
        }
        void Stop()
        {
            ServerState::state(m_key, TimerHelper::Now_Millisecond() - m_startms);
            m_startms = TimerHelper::Now_Millisecond();
        }
        ~StateTimeMs()
        {
            if (m_autoState) {
                ServerState::state(m_key, TimerHelper::Now_Millisecond() - m_startms);
            }
        }

    private:
        std::string m_key;
        uint64_t m_startms;
        bool m_autoState;
    };

private:

    union Status {

        struct {
            float minvalue;
            float maxvalue;
            float avgvalue;
            uint32_t num;
        } stater;

        struct{
            uint64_t a;
            uint64_t b;
        } atomic;
    };



public:
    static std::map<std::string, AtomicIntegerT<uint64_t> > countermap0;
    static std::map<std::string, AtomicIntegerT<uint64_t> > countermap1;
    static std::map<std::string, Status > statusmap;
    static volatile int dumpinternalms;
    static MutexLock mutex;
    static mtrpc::Worker* worker;
};
}
#endif


