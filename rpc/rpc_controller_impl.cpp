#include "rpc_controller_impl.h"
#include "proto/rpc_option.pb.h"
#include "mio/mio_error_code.h"

namespace mtrpc {



RpcControllerImpl::RpcControllerImpl():
    RpcController(),
    _msg(""),
    _status(-1),

    _req_send_size(0),
    _req_pack_size(0),
    _res_send_size(0),
    _res_pack_size(0),

    _request(NULL),
    _response(NULL),
    _stream(NULL),
    _poller(NULL)
{

}

RpcControllerImpl::~RpcControllerImpl(){

}


void RpcControllerImpl::Reset()
{

    _msg = "";
    _status = -1;

    _req_send_size = 0;
    _req_pack_size = 0;
    _res_send_size = 0;
    _res_pack_size = 0;

    _request = NULL;
    _response= NULL;
    _stream  = NULL;
    _poller  = NULL;
}


bool RpcControllerImpl::Failed() const {

    return _status != OK;
}



int RpcControllerImpl::ErrorCode() const{

    return _status;
}


std::string RpcControllerImpl::ErrorText() const
{
    return _msg;
}


void RpcControllerImpl::SetFailed(const std::string& reason)
{
    _msg = reason;
}


void RpcControllerImpl::SetStatus(int status)
{
    _status = status;
    _msg =  ErrorString(_status);
}

////
RpcClientController::RpcClientController():
RpcControllerImpl(),
_method(NULL),
_done(NULL)
{
}

RpcClientController::~RpcClientController()
{

}



void RpcClientController::Reset(){
    RpcControllerImpl::Reset();
    _method = NULL;
    _done = NULL;

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
RpcControllerImpl(){

}



}
