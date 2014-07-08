#include "rpc_controller.h"
#include "proto/rpc_option.pb.h"

namespace mtrpc {

RpcController::RpcController(){

}


RpcController::~RpcController(){

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

     return true;

}



int RpcController::ErrorCode() const{

    return 0;
}


std::string RpcController::ErrorText() const
{

    return 0;
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

}


bool RpcController::IsCanceled() const{

    return true;
}



void RpcController::NotifyOnCancel(google::protobuf::Closure* callback){

}




void RpcController::Wait(){

}


uint32_t RpcController::GetSeq(){

    return 1;
}

}
