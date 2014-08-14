#include "stdio.h"
#include "log/log.h"

#include "rpc/rpc_client.h"
#include "proto/builtin_service.pb.h"
#include "common/signalhelper.h"
#include "proto/face.pb.h"

using namespace mtrpc;
using namespace youtu;


int main(int argc,char*argv[]){

    //SignalHelper::install_sig_action();

    const std::string addr = "127.0.0.1:8000";


    RpcClient client;

    RpcChannel* channel = client.GetChannel(addr);
    TRACE("get channel ok"<<channel);


    if(channel){

        RpcController* cntl = channel->GetController();

         FaceImportServer_Stub stub(channel);

        ::youtu::FaceImportRequest req;
        ::youtu::FaceImportResponse res;

         req.set_uin(0);
        stub.Import(cntl,&req,&res,NULL);
        std::cout<<"error:"<<cntl->ErrorText()<<std::endl;
        cntl->Reset();

        std::cout<<",result:"<<res.faceid_size()<<std::endl;
        delete cntl;
    }

    client.ReleaseChannel(channel);
    client.Shutdown();
    client.Join();

    return 0;

}
