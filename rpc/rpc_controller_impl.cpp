#include "rpc_controller_impl.h"
#include "proto/rpc_option.pb.h"

namespace mtrpc {

RpcControllerImpl::RpcControllerImpl():
_status(-1){

}


RpcControllerImpl::~RpcControllerImpl(){

}




void RpcControllerImpl::Wait()
{

    while(_status == -1)
    {
        WriteLock<MutexLock> lock(mutex);
        cv.wait(&mutex._mutex);
    }
}


void RpcControllerImpl::SetStatus(int status)
{
    WriteLock<MutexLock> lock(mutex);
    _status = status;
    cv.notifyOne();
}



}
