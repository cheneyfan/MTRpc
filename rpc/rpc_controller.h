#ifndef _MTRPC_RPC_CONTROLLER_H_
#define _MTRPC_RPC_CONTROLLER_H_

#include <google/protobuf/service.h>
#include "proto/rpc_option.pb.h"



namespace mtrpc {

typedef google::protobuf::int64 int64;
typedef ::google::protobuf::Message Message;

class RpcControllerImpl;
class SocketStream;
class Epoller;

class RpcController :
        public google::protobuf::RpcController
{
public:
    RpcController();
    virtual ~RpcController();

    virtual void Reset(){}

    /// Not implement
    virtual std::string LocalAddress() const{return "";}
    virtual std::string RemoteAddress() const{return "";}

    virtual void SetTimeout(int64 timeout_in_ms){}
    virtual int64 Timeout() const{return 0;}

    virtual void SetRequestCompressType(CompressType compress_type){}
    virtual void SetResponseCompressType(CompressType compress_type){}

    virtual void StartCancel(){}

    virtual bool IsCanceled() const{return false;}

    virtual void NotifyOnCancel(google::protobuf::Closure* callback){}

    ////
    virtual void SetStatus(int status){}
    virtual void SetFailed(const std::string& reason){}
    virtual  uint64_t GenerateSeq();

}; // class RpcController

}

#endif
