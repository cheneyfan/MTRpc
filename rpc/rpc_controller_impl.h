#ifndef _MTRPC_RPC_CONTROLLER_IMPL_H_
#define _MTRPC_RPC_CONTROLLER_IMPL_H_

#include <google/protobuf/service.h>
#include "proto/rpc_option.pb.h"

#include "rpc_controller.h"
#include "common/rwlock.h"

namespace mtrpc {

typedef google::protobuf::int64 int64;



class RpcClientController: public RpcController {
public:
    RpcClientController();
    virtual ~RpcClientController();



    bool IsRequestSent() const;

    virtual void StartCancel();
    // final "done" callback.
    virtual bool IsCanceled() const;

    virtual void NotifyOnCancel(google::protobuf::Closure* callback);
    ///
    /// \brief Wait
    ///
    void Wait();

public:
    MutexLock mutex;
    ConditionVariable cv;

    ::google::protobuf::MethodDescriptor* method;
    ::google::protobuf::Closure* done;
};


class RpcServerController: public RpcController {
public:

    RpcServerController();

};


}

#endif
