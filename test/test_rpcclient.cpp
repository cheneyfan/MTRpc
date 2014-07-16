#include "stdio.h"
#include "log/log.h"

#include "rpc/rpc_client.h"
#include "proto/builtin_service.pb.h"
#include "common/signalhelper.h"
using namespace mtrpc;



int main(int argc,char*argv[]){

    SignalHelper::install_sig_action();

    const std::string addr = "127.0.0.1:8000";


    RpcClient client;

    RpcChannel* channel = client.GetChannel(addr);



    if(channel){

        RpcController* cntl = channel->GetController();

        builtin::BuiltinService_Stub stub(channel);

        ::mtrpc::builtin::HealthRequest req;
        ::mtrpc::builtin::HealthResponse res;
         stub.Health(cntl,&req,&res,NULL);

        if(cntl->Failed())
        {

        }


        std::cout<<"result:"<<res.health()<<std::endl;
    }

    client.ReleaseChannel(channel);
    return 0;

}
