#include "stdio.h"
#include "log/log.h"
#include "rpc/rpc_server.h"
#include "common/signalhelper.h"
#include "proto/face.pb.h"
#include "common/signalhelper.h"

using namespace mtrpc;
using namespace youtu;

class A : public FaceImportServer{
public:

    virtual void Import(::google::protobuf::RpcController* controller,
                         const ::youtu::FaceImportRequest* request,
                         ::youtu::FaceImportResponse* response,
                         ::google::protobuf::Closure* done){

       /* TRACE("sleep 10 secnd");
        //sleep(10);

        char buffer[2048]={0};

        SignalHelper::getBacktrace(buffer,2048,0,5);
        printf("buffer:%s",buffer);

        response->add_faceid(0);
      */
    }
};

int main(int argc,char*argv[]){
    Json::Value conf;
    //LogBacker::Init(conf);

    RpcServerOptions opt;

    opt.work_thread_num = atoi(argv[1]);
    std::cout<<"start :"<<opt.work_thread_num<<std::endl;

    RpcServer svr(opt);


    //SignalHelper::registerCallbak<RpcServer,&RpcServer::Stop>(&svr);

    svr.RegisterService(new A());
    svr.Start("127.0.0.1:8000");
    svr.Join();
    return 0;

}
