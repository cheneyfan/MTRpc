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

    virtual void Reset();

    /// Not implement
    std::string LocalAddress() const{}
    std::string RemoteAddress() const{}

    void SetTimeout(int64 timeout_in_ms){}
    int64 Timeout() const{}

    void SetRequestCompressType(CompressType compress_type){}
    void SetResponseCompressType(CompressType compress_type){}

    /// Status of rpc call
    virtual bool Failed() const;
    virtual int ErrorCode() const;
    virtual std::string ErrorText() const;

    virtual void SetStatus(int status);
    virtual void SetFailed(const std::string& reason);

    uint64_t GetSeq();

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

}; // class RpcController

}

#endif
