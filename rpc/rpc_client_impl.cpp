
#include "mio/mio_poller.h"
#include "thread/workpool.h"
#include "rpc_client_impl.h"
#include "rpc_channel.h"
#include "rpc_channel_impl.h"

namespace mtrpc {


RpcClientImpl::RpcClientImpl(const RpcClientOptions& opt):
_options(opt){
    _poller = new Epoller();
    _group  = new WorkGroup();
}

RpcClientImpl::~RpcClientImpl(){

    delete _poller;
    delete _group;
}

void RpcClientImpl::Start(){

    _group->Init(2);
    MioTask* task = NewExtClosure(_poller,&Epoller::Poll);
    _group->Post(task);
}

void RpcClientImpl::Stop(){

    _poller->Stop();

    _group->Stop();
}

RpcClientOptions RpcClientImpl::GetOptions()
{
    return _options;
}


void RpcClientImpl::ResetOptions(const RpcClientOptions& options){

    _options = options;
}

int RpcClientImpl::ConnectionCount(){
    return 0;
}


RpcChannel* RpcClientImpl::GetChannel(const std::string& server_addr){

     RpcChannel* channel =  new RpcChannel();

     channel->_impl->_poller = _poller;
     channel->_impl->_group  = _group;



     char host[64]= {0};
     int  port = 0;

     sscanf(server_addr.c_str(),"%[^:]:%d", host, &port);

     if( channel->Connect(host,port) != 0 ){
         delete channel;
         return NULL;
     }

     return channel;
}


void RpcClientImpl::ReleaseChannel(RpcChannel *channel)
{
    delete channel;
}


};

