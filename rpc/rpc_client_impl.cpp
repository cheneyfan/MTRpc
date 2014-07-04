#include "rpc_client_impl.h"
#include "mio/mio_poller.h"
#include "thread/workpool.h"

namespace mtrpc {


RpcClientImpl::RpcClientImpl(const RpcClientOptions& opt):
_options(opt){

    poller = new Epoller();
    group  = new WorkGroup();
    Start();
}

void RpcClientImpl::Start(){

    group->Init(1);
    MioTask* task = NewExtClosure(poller,&Epoller::Poll);
    group->Post(task);
}

void RpcClientImpl::Stop(){

    poller->Stop();

    group->Stop();
}

RpcClientOptions RpcClientImpl::GetOptions()
{
    return _options;
}

int RpcClientImpl::ConnectionCount(){
    return 0;
}



};

