#ifndef _MTRPC_RPC_CONTROLLER_IMPL_H_
#define _MTRPC_RPC_CONTROLLER_IMPL_H_

#include <google/protobuf/service.h>
#include "proto/rpc_option.pb.h"

#include "rpc_controller.h"
#include "common/rwlock.h"

namespace mtrpc {

typedef google::protobuf::int64 int64;



class RpcControllerImpl {
public:
    RpcControllerImpl();
    virtual ~RpcControllerImpl();


    ///
    /// \brief Wait
    ///
    void Wait();


    ///
    /// \brief SetStatus
    /// \param status
    ///
    void SetStatus(int status);


public:

    MutexLock mutex;
    ConditionVariable cv;

    std::string _msg;
    volatile int  _status;
};

}

#endif
