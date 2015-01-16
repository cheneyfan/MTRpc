#include "stdio.h"
#include "log/log.h"

#include "rpc/rpc_client.h"

#include "common/signalhelper.h"
#include "proto/face.pb.h"
#include "common/serverstat.h"

using namespace mtrpc;
using namespace youtu;


void hehe(){

}

void Test(RpcClient* client)
{


    const std::string addr = "10.12.191.45:8081";
    RpcChannelOptions opt;
    opt.read_timeout_sec = 3;
    RpcChannel* channel = client->GetChannel(addr, opt);
    TRACE("get channel ok"<<channel);


    if(channel){

        RpcController* cntl = channel->GetController();

        FaceImportServer_Stub stub(channel);
        {
            if(true){
                ::youtu::FaceImportRequest req;
                ::youtu::FaceImportResponse res;
                ServerState::StateTimeMs("test");
                req.set_uin(10000);
                youtu::FaceItem* item = req.add_items();

                item->set_faceid(100);
                item->set_photoid("hehe");
                item->set_x(0);
                item->set_y(1);
                item->set_w(2);
                item->set_h(3);


                stub.Import(cntl,&req,&res, NULL);

                if(cntl->Failed())
                {
                    TRACE("cntl->Failed:"<<cntl->ErrorCode()<<","<<cntl->ErrorText());
                    std::cout<<time(NULL)<<"cntl->Failed:"<<cntl->ErrorCode()<<","<<cntl->ErrorText()<<std::endl;
                    delete cntl;
                    client->ReleaseChannel(channel);
                    return ;
                }

                std::cout<<time(NULL)<<",res num:"<<res.faceid_size()<<std::endl;
                cntl->Reset();
            }
        }

        std::cout<<"error:"<<cntl->ErrorText()<<std::endl;
        delete cntl;
    }

    client->ReleaseChannel(channel);
}


int main(int argc,char*argv[]){

    ServerState::registerStatus("test");
    ServerState::startDumper("s.log");


    RpcClientOptions opt;
    opt.recv_timeout_sec = opt.send_timeout_sec= 5;

    opt.work_thread_num = 1;
    if(argc > 1)
         opt.work_thread_num = atoi(argv[1]);

    std::cout<<"start :"<<opt.work_thread_num<<std::endl;

    RpcClient* client = new RpcClient(opt);

    WorkGroup group;
    group.Init( opt.work_thread_num);
    for(int i=0;i<opt.work_thread_num;++i)
        group.Post(NewExtClosure(Test, client));

    group.join();

    client->Shutdown();
    client->Join();
    delete client;
    return 0;

}
