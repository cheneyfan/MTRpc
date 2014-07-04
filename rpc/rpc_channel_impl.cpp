#include "rpc_channel_impl.h"
#include "mio/mio_error_code.h"
#include "mio/mio_connect_stream.h"
namespace mtrpc {

RpcChannelImpl::RpcChannelImpl(const RpcClientOptions& options):
_options(options){
}


bool RpcChannelImpl::IsAddressValid(){
    return true;
}

int RpcChannelImpl::Connect(const std::string& server_ip,int32_t server_port){

   int ret = _stream->Connect(server_ip, server_port);

   if(ret == 0)
   {
       _stream->AddEventASync(_poller,true,false);
   }else if(ret == EINPROGRESS){

       _stream->AddEventASync(_poller,true,false);
   }


}



}
