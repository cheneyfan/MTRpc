#include "rpc_controller.h"
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

    static AtomicInt32 seq;
    return seq.incrementAndGet();
}

}
