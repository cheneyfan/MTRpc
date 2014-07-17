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


        int i =0;
        char buf[32]={0};

        snprintf(buf,32,"hello_%d_",i);

        req.set_health(buf);

        stub.Health(cntl,&req,&res,NULL);
        std::cout<<"error:"<<cntl->ErrorText()<<std::endl;
        cntl->Reset();

        std::cout<<i<<",result:"<<res.health()<<std::endl;

        i = 1;

        snprintf(buf, 32, "hello_%d_", i);

        req.set_health(buf);

        stub.Health(cntl,&req,&res,NULL);

        std::cout<<"error:"<<cntl->ErrorText()<<std::endl;
        cntl->Reset();
        std::cout<<i<<",result:"<<res.health()<<std::endl;

        delete cntl;
    }

    client.ReleaseChannel(channel);
    client.Join();

    return 0;

}
