#include "stdio.h"
#include "log/log.h"

#include "rpc/rpc_client.h"
#include "proto/builtin_service.pb.h"
#include "common/signalhelper.h"
#include "proto/face.pb.h"
#include "common/serverstat.h"

using namespace mtrpc;
using namespace youtu;




void Test(RpcClient& client)
{


    const std::string addr = "127.0.0.1:8000";
    RpcChannel* channel = client.GetChannel(addr);
    TRACE("get channel ok"<<channel);


    if(channel){

        RpcController* cntl = channel->GetController();

        FaceImportServer_Stub stub(channel);

        ::youtu::FaceImportRequest req;
        ::youtu::FaceImportResponse res;

        req.set_uin(0);
        {
            while(true){
                ServerState::StateTimeMs("test");
                stub.Import(cntl,&req,&res,NULL);
                cntl->Reset();
            }
        }
        std::cout<<"error:"<<cntl->ErrorText()<<std::endl;


        std::cout<<",result:"<<res.faceid_size()<<std::endl;
        delete cntl;
    }

    client.ReleaseChannel(channel);
}


int main(int argc,char*argv[]){

    ServerState::registerStatus("test");
    ServerState::startDumper("s.log");


    RpcClientOptions opt;
    opt.work_thread_num = atoi(argv[1]);
    std::cout<<"start :"<<opt.work_thread_num<<std::endl;

    RpcClient client(opt);

    WorkGroup group;
    group.Init( opt.work_thread_num);
    for(int i=0;i<opt.work_thread_num;++i)
        group.Post(NewExtClosure(Test,client));

    group.join();

    client.Shutdown();
    client.Join();

    return 0;

}
