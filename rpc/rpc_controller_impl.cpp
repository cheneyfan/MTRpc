#include "rpc_controller_impl.h"
#include "proto/rpc_option.pb.h"

namespace mtrpc {

RpcClientController::RpcClientController():
RpcController(){
}

RpcClientController::~RpcClientController()
{

}

void RpcClientController::Wait()
{
    while(_status == -1)
    {
        WriteLock<MutexLock> lock(mutex);
        cv.wait(&mutex._mutex);
    }
}


void RpcClientController::SetStatus(int status)
{
    WriteLock<MutexLock> lock(mutex);
    _status = status;
    cv.notifyOne();
}


RpcServerController:: RpcServerController():
RpcController(){
}



}
