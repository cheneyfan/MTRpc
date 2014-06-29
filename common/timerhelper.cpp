// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: <guofutan@tencent.com>
// Created: 2013-07-01
// Description:

#include "timerhelper.h"
#include "log/log.h"

namespace mtrpc {



TimeMoniter:: ~TimeMoniter(){

    uint64_t used = TimerHelper::Now_Millisecond() - start;

    if( used >= _ms)
    {
        WARN("long time: "<<used<<"ms,"<<buf.str());
    }
}

}
