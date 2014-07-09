#include "rpc_controller.h"
#include "proto/rpc_option.pb.h"
#include "rpc_controller_impl.h"
#include "mio/mio_error_code.h"

namespace mtrpc {

RpcController::RpcController():
    _impl(new RpcControllerImpl())
{
}


RpcController::~RpcController(){

    delete _impl;
}


std::string RpcController::LocalAddress() const{

    return "";
}


std::string  RpcController::RemoteAddress() const{

    return "";
}

void RpcController::Reset(){

}


void RpcController::SetTimeout(int64 timeout_in_ms){

}

int64 RpcController::Timeout() const{

    return 0;
}



void RpcController::SetRequestCompressType(CompressType compress_type){

}



void RpcController::SetResponseCompressType(CompressType compress_type){

}



bool RpcController::Failed() const{

    return _impl->_status != RPC_SUCCESS;

}



int RpcController::ErrorCode() const{

    return _impl->_status;
}


std::string RpcController::ErrorText() const
{

    return _impl->_msg;
}


bool RpcController::IsRequestSent() const{

    return true;
}

int64 RpcController::SentBytes() const{

    return 0;
}

void RpcController::StartCancel(){


}


void RpcController::SetFailed(const std::string& reason){

    _impl->_msg = reason;
}


bool RpcController::IsCanceled() const{

    return true;
}



void RpcController::NotifyOnCancel(google::protobuf::Closure* callback){

}




void RpcController::Wait(){

    _impl->Wait();
}


uint32_t RpcController::GetSeq(){

    return 1;
}


void RpcController::SetStatus(int status)
{
    _impl->SetStatus(status);
}

}
