#include "stdio.h"
#include "log/log.h"
#include "rpc/rpc_server.h"

using namespace mtrpc;



int main(int argc,char*argv[]){

    Json::Value conf;
    LogBacker::Init(conf);

    RpcServerOptions opt;

    RpcServer svr(opt);
    svr.Start("127.0.0.1:8000");
    svr.Join();
    return 0;

}
