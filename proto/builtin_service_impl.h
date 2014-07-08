

#ifndef _MTRPC_BUILTIN_SERVICE_IMPL_H_
#define _MTRPC_BUILTIN_SERVICE_IMPL_H_


#include "builtin_service.pb.h"

namespace mtrpc {

namespace builtin {

class BuiltinServiceImpl : public BuiltinService {
public:
    BuiltinServiceImpl();
    virtual ~BuiltinServiceImpl();

    virtual void Health(::google::protobuf::RpcController* controller,
            const ::mtrpc::builtin::HealthRequest* request,
            ::mtrpc::builtin::HealthResponse* response,
            ::google::protobuf::Closure* done);

    virtual void ServerOptions(::google::protobuf::RpcController* controller,
            const ::mtrpc::builtin::ServerOptionsRequest* request,
            ::mtrpc::builtin::ServerOptionsResponse* response,
            ::google::protobuf::Closure* done);

    virtual void UpdateOptions(::google::protobuf::RpcController* controller,
            const ::mtrpc::builtin::UpdateOptionsRequest* request,
            ::mtrpc::builtin::UpdateOptionsResponse* response,
            ::google::protobuf::Closure* done);

    virtual void ServerStatus(::google::protobuf::RpcController* controller,
            const ::mtrpc::builtin::ServerStatusRequest* request,
            ::mtrpc::builtin::ServerStatusResponse* response,
            ::google::protobuf::Closure* done);

    virtual void ListService(::google::protobuf::RpcController* controller,
            const ::mtrpc::builtin::ListServiceRequest* request,
            ::mtrpc::builtin::ListServiceResponse* response,
            ::google::protobuf::Closure* done);

    virtual void Stat(::google::protobuf::RpcController* controller,
            const ::mtrpc::builtin::StatRequest* request,
            ::mtrpc::builtin::StatResponse* response,
            ::google::protobuf::Closure* done);


};

} //
} //

#endif //

/* vim: set ts=4 sw=4 sts=4 tw=100 */
