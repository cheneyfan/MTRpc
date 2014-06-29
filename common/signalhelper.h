// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: <guofutan@tencent.com>
// Created: 2013-07-01
// Description:

/// a helper for register signal callback
///
///

#ifndef  _SIGNAL_HELPER_H_
#define  _SIGNAL_HELPER_H_

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/prctl.h>

#include <vector>

#include "thread/workpool.h"

namespace  MTRpc {



class SignalHelper
{
public:
    // sigaction needs this fuction
    static void signal_handler(int signal);


    // sigaction needs this fuction
    static void core_handler(int signal);

    ///
    /// \brief install_sig_action
    /// \return
    ///
    static int install_sig_action();

    ///
    /// \brief enable_core_dump
    /// \param enabled
    /// \param core_file_size
    /// \return
    ///
    static int enable_core_dump(bool enabled, int core_file_size);

    ///
    /// \brief registerCallback
    ///
     template<class A,void(A::*f)(void)>
    static void registerCallback(A* o){
        callbacks.insert(callbacks.begin(),Closure::From<A,f>(o));
    }

    ///
    /// \brief registerCallback
    ///
    static void registerCallback(void(*f)(void*)){
        callbacks.insert(callbacks.begin(),Closure::From(f,NULL));
    }


    ///
    /// \brief getBacktrace
    /// \param buffer
    /// \param size
    /// \param start
    /// \param num
    ///
    static void getBacktrace(char* buffer,int size,int start,int num);
public:
    static std::vector<Closure> callbacks;
};
}

#endif
