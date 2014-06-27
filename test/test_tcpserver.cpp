#include "stdio.h"
#include "mio2/log.h"
#include "mio2/workpool.h"
#include "mio2/signalhelper.h"
#include "mio2/mio_tcpserver.h"
#include "mio2/mio_handler.h"
using namespace mio2;

int main(int argc,char*argv[]){


    Json::Value conf;
    mio2::LogBacker::Init(conf);

    TcpServer ts;
    ts.Init<mio2::ConnectHandler>(conf);
    ts.Start();
    ts.Join();
}
