#ifndef _MTRPC_RPC_CONTROLLER_IMPL_H_
#define _MTRPC_RPC_CONTROLLER_IMPL_H_

#include <google/protobuf/service.h>
#include "proto/rpc_option.pb.h"

#include "rpc_controller.h"
#include "common/rwlock.h"

namespace mtrpc {

typedef google::protobuf::int64 int64;

class RpcControllerImpl : public  RpcController{
public:

    RpcControllerImpl();

    virtual ~RpcControllerImpl();

    virtual void Reset();

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

    /// Status of rpc call
    virtual bool Failed() const;
    virtual int ErrorCode() const;

    virtual std::string ErrorText() const;

    virtual void SetStatus(int status);
    virtual void SetFailed(const std::string& reason);

    virtual uint64_t GetSeq();





public:

    std::string _msg;
    volatile int  _status;

    uint32_t _req_send_size;
    uint32_t _req_pack_size;

    uint32_t _res_send_size;
    uint32_t _res_pack_size;

    Message* _request;
    Message* _response;
    SocketStream * _stream;
    Epoller * _poller;
};


class RpcClientController: public RpcControllerImpl {
public:
    RpcClientController();

    virtual ~RpcClientController();

    virtual void Reset();



    virtual void SetStatus(int status);
    ///
    /// \brief Wait the call done
    ///
    void Wait();




public:
    MutexLock mutex;
    ConditionVariable cv;

    ::google::protobuf::MethodDescriptor* _method;
    ::google::protobuf::Closure* _done;
};


class RpcServerController: public RpcControllerImpl {
public:
    RpcServerController();

};


}

#endif
