#include <builtin_service_impl.h>

namespace mtrpc {

namespace builtin {



BuiltinServiceImpl::BuiltinServiceImpl()
{
}

BuiltinServiceImpl::~BuiltinServiceImpl()
{
}

void BuiltinServiceImpl::Health(::google::protobuf::RpcController* /* controller */,
        const ::mtrpc::builtin::HealthRequest* /* request */,
        ::mtrpc::builtin::HealthResponse* response,
        ::google::protobuf::Closure* done)
{
    done->Run();
}

void BuiltinServiceImpl::ServerOptions(::google::protobuf::RpcController* controller,
        const ::mtrpc::builtin::ServerOptionsRequest* /* request */,
        ::mtrpc::builtin::ServerOptionsResponse* response,
        ::google::protobuf::Closure* done)
{
    done->Run();
}

void BuiltinServiceImpl::UpdateOptions(::google::protobuf::RpcController* controller,
        const ::mtrpc::builtin::UpdateOptionsRequest* request,
        ::mtrpc::builtin::UpdateOptionsResponse* response,
        ::google::protobuf::Closure* done)
{
   done->Run();
}

void BuiltinServiceImpl::ServerStatus(::google::protobuf::RpcController* controller,
        const ::mtrpc::builtin::ServerStatusRequest* /* request */,
        ::mtrpc::builtin::ServerStatusResponse* response,
        ::google::protobuf::Closure* done)
{
    done->Run();
}

void BuiltinServiceImpl::ListService(::google::protobuf::RpcController* controller,
        const ::mtrpc::builtin::ListServiceRequest* /* request */,
        ::mtrpc::builtin::ListServiceResponse* response,
        ::google::protobuf::Closure* done)
{
    done->Run();
}

void BuiltinServiceImpl::Stat(::google::protobuf::RpcController* controller,
        const ::mtrpc::builtin::StatRequest* request,
        ::mtrpc::builtin::StatResponse* response,
        ::google::protobuf::Closure* done)
{
    done->Run();
}

} // namespace builtin
} // namespace

/* vim: set ts=4 sw=4 sts=4 tw=100 */
