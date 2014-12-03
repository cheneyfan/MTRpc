// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#ifndef _MTRPC_SERVICE_POOL_H_
#define _MTRPC_SERVICE_POOL_H_

#include <map>
#include <set>
#include <list>

#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>




#include "common/murmurhash.h"
#include "log/log.h"

#include "common/rwlock.h"
#define SERVICE_CACHE_SLOT_COUNT 1019
#define STAT_SLOT_COUNT 602
#define STAT_SLOT_SECONDS 1
#include <google/protobuf/service.h>

namespace mtrpc {

typedef google::protobuf::int64 int64;
typedef google::protobuf::uint64 uint64;


// All the time is in micro-seconds.
struct StatSlot
{
    int64 succeed_count;
    int64 succeed_process_time;
    int64 succeed_max_process_time;
    int64 failed_count;
    int64 failed_process_time;
    int64 failed_max_process_time;
};

class MethodBoard
{
public:
    MethodBoard() : _desc(NULL), _slot_index(0)
    {
        memset(_stat_slots, 0, sizeof(_stat_slots));
    }

    ~MethodBoard() {}

    void SetDescriptor(const google::protobuf::MethodDescriptor* desc)
    {
        _desc = desc;
    }

    const google::protobuf::MethodDescriptor* Descriptor()
    {
        return _desc;
    }

    void ReportProcessBegin()
    {
    }

    void ReportProcessEnd(bool succeed, int64 time_in_us)
    {
        volatile StatSlot* slot = &_stat_slots[_slot_index];
        if (succeed) {
            ++slot->succeed_count;
            slot->succeed_process_time += time_in_us;
            if (slot->succeed_max_process_time < time_in_us) {
                slot->succeed_max_process_time = time_in_us;
            }
        }
        else {
            ++slot->failed_count;
            slot->failed_process_time += time_in_us;
            if (slot->failed_max_process_time < time_in_us) {
                slot->failed_max_process_time = time_in_us;
            }
        }
    }

    void NextSlot()
    {
        _slot_index = ((_slot_index + 1 == STAT_SLOT_COUNT) ?  0 : _slot_index + 1);
        memset(&_stat_slots[_slot_index], 0, sizeof(StatSlot));
    }

    void LatestStats(int slot_count, StatSlot* stat_out)
    {
        SCHECK(slot_count > 0 && slot_count <= STAT_SLOT_COUNT);
        SCHECK(stat_out != NULL);
        memset(stat_out, 0, sizeof(StatSlot));
        int index = (_slot_index == 0 ? STAT_SLOT_COUNT - 1 : _slot_index - 1);
        while (slot_count > 0) {
            volatile StatSlot* slot = &_stat_slots[index];
            stat_out->succeed_count += slot->succeed_count;
            stat_out->succeed_process_time += slot->succeed_process_time;
            if (stat_out->succeed_max_process_time < slot->succeed_max_process_time) {
                stat_out->succeed_max_process_time = slot->succeed_max_process_time;
            }
            stat_out->failed_count += slot->failed_count;
            stat_out->failed_process_time += slot->failed_process_time;
            if (stat_out->failed_max_process_time < slot->failed_max_process_time) {
                stat_out->failed_max_process_time = slot->failed_max_process_time;
            }
            --slot_count;
            index = (index == 0 ? STAT_SLOT_COUNT - 1 : index - 1);
        }
    }


private:
    const google::protobuf::MethodDescriptor* _desc;
    StatSlot _stat_slots[STAT_SLOT_COUNT];
    volatile int _slot_index;
};

class ServiceBoard
{
public:
    ServiceBoard(ServiceBoard* next, google::protobuf::Service* svc, bool own)
        : _next(next), _svc(svc), _own(own)
    {
        _svc_desc = _svc->GetDescriptor();
        _method_count = _svc_desc->method_count();
        _method_boards = new MethodBoard[_method_count];
        for (int i = 0; i < _method_count; ++i) {
            _method_boards[i].SetDescriptor(_svc_desc->method(i));
        }
    }

    ~ServiceBoard()
    {
        delete[] _method_boards;
        if (_own) {
            delete _svc;
        }
    }

    ServiceBoard* Next()
    {
        return _next;
    }

    const std::string& ServiceName()
    {
        return _svc_desc->full_name();
    }

    google::protobuf::Service* Service()
    {
        return _svc;
    }

    MethodBoard* Method(int method_id)
    {
        SCHECK(method_id >=0 && method_id < _method_count);
        return &_method_boards[method_id];
    }

    void ReportProcessBegin(int method_id)
    {
        SCHECK(method_id >=0 && method_id < _method_count);
        _method_boards[method_id].ReportProcessBegin();
    }

    void ReportProcessEnd(int method_id, bool succeed, int64 time_in_us)
    {
        SCHECK(method_id >=0 && method_id < _method_count);
        _method_boards[method_id].ReportProcessEnd(succeed, time_in_us);
    }

    void NextSlot()
    {
        for (int i = 0; i < _method_count; ++i) {
            _method_boards[i].NextSlot();
        }
    }

    void LatestStats(int method_id, int slot_count, StatSlot* stat_out)
    {
        SCHECK(method_id >=0 && method_id < _method_count);
        _method_boards[method_id].LatestStats(slot_count, stat_out);
    }




private:
    void GetRealPeriod(int64 expect_period, int64* real_period, int* slot_count)
    {
        int sc = 1;
        if (expect_period > 1) {
            sc = (expect_period - 1) / STAT_SLOT_SECONDS + 1;
        }
        sc = std::min(sc, STAT_SLOT_COUNT - 2);
        *real_period = sc * STAT_SLOT_SECONDS;
        *slot_count = sc;
    }

public:
    ServiceBoard* _next;
    google::protobuf::Service* _svc;
    bool _own;
    const google::protobuf::ServiceDescriptor* _svc_desc;
    int _method_count;
    MethodBoard* _method_boards;
};

class ServicePool
{
public:
    ServicePool() : _head(NULL), _count(0)
    {
        memset(_cache, 0, sizeof(_cache));
    }

    virtual ~ServicePool()
    {
        for (ServiceMap::iterator it = _service_map.begin();
                it != _service_map.end(); ++it) {
            delete it->second;
        }
    }

    bool RegisterService(google::protobuf::Service* service, bool take_ownership = true)
    {
        SCHECK(service != NULL);
        const std::string& svc_name = service->GetDescriptor()->full_name();
        WriteLock<MutexLock> _(_service_map_lock);
        // check exist
        if (_service_map.find(svc_name) != _service_map.end()) {
            return false;
        }
        TRACE("register :"<<svc_name);
        // register service
        ServiceBoard* svc_board = new ServiceBoard(_head, service, take_ownership);
        _service_map[svc_name] = svc_board;
        _head = svc_board;
        ++_count;
        // add into cache
        uint64 cache_index = CacheIndex(svc_name);
        if (_cache[cache_index] == NULL) {
            _cache[cache_index] = svc_board;
        }
        return true;
    }

    ServiceBoard* FindService(const std::string& svc_name)
    {
        // find in cache first
        uint64 cache_index = CacheIndex(svc_name);
        if (_cache[cache_index] != NULL
                && _cache[cache_index]->ServiceName() == svc_name) {
            return _cache[cache_index];
        }
        // find in map then
        WriteLock<MutexLock> _(_service_map_lock);
        ServiceMap::iterator it = _service_map.find(svc_name);
        return it == _service_map.end() ? NULL : it->second;
    }

    // List registered services in order by name.
    void ListService(std::list<ServiceBoard*>* svc_list)
    {
        SCHECK(svc_list != NULL);
        svc_list->clear();
        // get services from link, and make them in order
        ServiceBoard* sp = _head;
        ServiceMap tmp_map;
        while (sp != NULL) {
            tmp_map[sp->ServiceName()] = sp;
            sp = sp->Next();
        }
        // copy to list
        for (ServiceMap::iterator it = tmp_map.begin();
                it != tmp_map.end(); ++it) {
            svc_list->push_back(it->second);
        }
    }

    // List registered services in order by name.
    void ListService(std::list<google::protobuf::Service*>* svc_list)
    {
        SCHECK(svc_list != NULL);
        svc_list->clear();
        std::list<ServiceBoard*> board_list;
        ListService(&board_list);
        for (std::list<ServiceBoard*>::iterator it = board_list.begin();
                it != board_list.end(); ++it) {
            svc_list->push_back((*it)->Service());
        }
  }

    int ServiceCount()
    {
        return _count;
    }

    void NextSlot()
    {
        ServiceBoard* sp = _head;
        while (sp != NULL) {
            sp->NextSlot();
            sp = sp->Next();
        }
    }

private:
    uint64 CacheIndex(const std::string& name)
    {
        return murmurhash(name.c_str(), name.size()) % SERVICE_CACHE_SLOT_COUNT;
    }



public:
    typedef std::map<std::string, ServiceBoard*> ServiceMap;
    ServiceMap _service_map;
    MutexLock _service_map_lock;

    ServiceBoard* _head;
    int _count;
    ServiceBoard* _cache[SERVICE_CACHE_SLOT_COUNT];

}; // class ServicePool


} // namespace mtrpc

#endif // _SOFA_PBRPC_SERVICE_POOL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */

