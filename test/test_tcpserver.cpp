#include <stdio.h>
#include "log/log.h"
#include "thread/workpool.h"
#include "common/signalhelper.h"
#include "mio/mio_tcpserver.h"
#include "mio/mio_handler.h"
using namespace mtrpc;

int main(int argc,char*argv[]){


    Json::Value conf;
    mtrpc::LogBacker::Init(conf);

    TcpServer ts;
    ts.Init<mtrpc::ConnectHandler>(conf);
    ts.Start();
    ts.Join();
}
