#include "stdio.h"
#include "log/log.h"
#include "rpc/rpc_server.h"
#include "common/signalhelper.h"
using namespace mtrpc;



int main(int argc,char*argv[]){
  SignalHelper::install_sig_action();
    Json::Value conf;
    //LogBacker::Init(conf);

    RpcServerOptions opt;

    opt.work_thread_num = 2;

    RpcServer svr(opt);
    svr.Start("127.0.0.1:8000");
    svr.Join();
    return 0;

}
