#include "rpc_controller.h"
#include "proto/rpc_option.pb.h"
#include "rpc_controller_impl.h"
#include "mio/mio_error_code.h"
#include "common/atomic.h"
namespace mtrpc {

RpcController::RpcController()
{

}


RpcController::~RpcController()
{

}


uint64_t RpcController::GenerateSeq(){

    static AtomicInt64 seq;
    return seq.incrementAndGet();
}

}
