#include "rpc_controller.h"
#include "proto/rpc_option.pb.h"
#include "rpc_controller_impl.h"
#include "mio/mio_error_code.h"

namespace mtrpc {

RpcController::RpcController()
{
    Reset();
}



virtual void RpcController::Reset()
{

    _msg = "";
    _status = -1;

    _packetsize = 0;
    _request = NULL;
    _response= NULL;
    _stream  = NULL;
    _poller  = NULL;
}

RpcController::~RpcController()
{

}



bool RpcController::Failed() const{

    return _status == OK;

}



int RpcController::ErrorCode() const{

    return _status;
}


std::string RpcController::ErrorText() const
{

    return _msg;
}


void RpcController::SetFailed(const std::string& reason)
{
    _msg = reason;
}

uint64_t RpcController::GetSeq(){

    uint64_t t = time(NULL);
    return  t<<32 | rand();
}


void RpcController::SetStatus(int status)
{
    _status = status;
    _msg =  ErrorString(_status);
}

}
