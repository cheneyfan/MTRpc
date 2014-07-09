#include "stdio.h"
#include "log/log.h"

#include "rpc/rpc_client.h"
#include "proto/builtin_service.pb.h"

using namespace mtrpc;



int main(int argc,char*argv[]){

    const std::string addr = "127.0.0.1:8000";


    RpcClient client;

    RpcChannel* channel = client.GetChannel(addr);

    RpcController* cntl = new RpcController();

    builtin::BuiltinService_Stub stub(channel);

    ::mtrpc::builtin::HealthRequest req;
    ::mtrpc::builtin::HealthResponse res;

    stub.Health(cntl,&req,&res,NULL);

    std::cout<<res.health()<<std::endl;

    return 0;

}
