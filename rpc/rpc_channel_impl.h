#ifndef _MTPRC_RPC_CHANNEL_IMPL_H_
#define _MTPRC_RPC_CHANNEL_IMPL_H_

#include <google/protobuf/service.h>
#include <smart_ptr/smart_ptr.hpp>


namespace mtrpc {
class RpcClient;
class RpcChannelImpl;



class RpcChannelImpl : public  mtrpc::enable_shared_from_this<RpcChannelImpl>
{
public:

    virtual ~RpcChannelImpl();

    bool IsAddressValid();

    void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                    ::google::protobuf::RpcController* controller,
                    const ::google::protobuf::Message* request,
                    ::google::protobuf::Message* response,
                    ::google::protobuf::Closure* done);


};

}
