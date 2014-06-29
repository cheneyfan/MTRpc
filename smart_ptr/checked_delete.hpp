// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

// This file is modified from boost.
//
// Copyright Beman Dawes 2002, 2006
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See library home page at http://www.boost.org/libs/system

#ifndef _MTRPC_SMART_PTR_CHECKED_DELETE_
#define _MTRPC_SMART_PTR_CHECKED_DELETE_

namespace mtrpc {

template <typename T>
inline void checked_delete(T* px)
{
    typedef char type_must_be_complete[sizeof(T) ? 1 : -1 ];
    (void) sizeof(type_must_be_complete);
    delete px;
}

template<class T> inline void checked_array_delete(T * x)
{
    typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
    (void) sizeof(type_must_be_complete);
    delete [] x;
}

template<class T> struct checked_deleter
{
    typedef void result_type;
    typedef T * argument_type;

    void operator()(T * x) const
    {
        ::mtrpc::checked_delete(x);
    }
};

template<class T> struct checked_array_deleter
{
    typedef void result_type;
    typedef T * argument_type;

    void operator()(T * x) const
    {
        ::mtrpc::checked_array_delete(x);
    }
};


} // namespace mtrpc

#endif // _MTRPC_SMART_PTR_CHECKED_DELETE_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
