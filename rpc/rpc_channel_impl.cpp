#include "rpc_channel.h"
#include "rpc_channel_impl.h"
#include "mio/mio_error_code.h"
#include "mio/mio_connect_stream.h"
#include "rpc_controller.h"

namespace mtrpc {

RpcChannelImpl::RpcChannelImpl(const RpcChannelOptions &options):
_options(options){

}


bool RpcChannelImpl::IsAddressValid(){
    return true;
}

int RpcChannelImpl::Connect(const std::string& server_ip,int32_t server_port){



    _stream = new ConnectStream();
    _stream->group = _group;
    _stream->handerMessageRecived =
            NewPermanentExtClosure(this,&RpcChannelImpl::OnMessageRecived);
    _stream->handerMessageSended =
            NewPermanentExtClosure(this,&RpcChannelImpl::OnMessageSended);


   int ret = _stream->Connect(server_ip, server_port);

   if(ret == 0)
   {
       _stream->AddEventASync(_poller,true,false);

       return 0;
   }else if(ret == EINPROGRESS){

       _stream->AddEventASync(_poller,true,false);

       return 0;
   }

   return -1;

}


void RpcChannelImpl::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                ::google::protobuf::RpcController* controller,
                const ::google::protobuf::Message* request,
                ::google::protobuf::Message* response,
                ::google::protobuf::Closure* done){


    if(_stream->_ConnectStatus == CONNECT_FAIL)
    {
        controller->SetFailed("Not Connect");
        done->Run();
        return ;
    }

    int size = 0;

    {
         WriteLock<MutexLock> lc(pendinglock);
         size = pendingcall.size();
    }

    if(size == 0 )
    {

        CallParams *currentCall = new CallParams();

        currentCall->method = method;
        currentCall->controller = controller;
        currentCall->request = request;
        currentCall->response = response;
        currentCall->done = done;
        pendingcall.push(currentCall);

        SendToServer(currentCall->method, currentCall->controller, currentCall->request);
    }

    //sync
    if(done == NULL)
    {
        RpcController * cntl = dynamic_cast<RpcController *>(controller);
        cntl->Wait();
    }
}

int RpcChannelImpl::SendToServer(const ::google::protobuf::MethodDescriptor* method,::google::protobuf::RpcController* controller,const ::google::protobuf::Message* request){

    RpcController* cntl =
            dynamic_cast<RpcController*>(controller);

    HttpHeader& reqheader = _stream->reqheader;

    WriteBuffer& buf = _stream->writebuf;

    reqheader.SetPath(method->full_name());
    reqheader.SetRequestSeq(cntl->GetSeq());

    WriteBuffer::Iterator it= buf.Reserve();


    WriteBuffer::Iterator mark = buf.writepos;
    request->SerializePartialToZeroCopyStream(&buf);

    _stream->packetEnd = buf.writepos;

    int content_length = buf.writepos - mark;
    reqheader.SetContentLength(content_length);
    reqheader.SerializeRequestHeader(it);

    _stream->ModEventAsync(_poller,true,true);
    return 0;
}

void RpcChannelImpl::OnMessageRecived(MessageStream* sream,Epoller* p){

    CallParams* params = NULL;
    {
         WriteLock<MutexLock> wl(pendinglock);
         params = pendingcall.front();
         pendingcall.pop();
    }

    params->response->ParseFromZeroCopyStream(&sream->readbuf);

    {
        WriteLock<MutexLock> wl(pendinglock);
        if(pendingcall.size() > 0 )
        {
             params =  pendingcall.front();
        }else{

            params =NULL;
        }
    }

    if(params == NULL)
        _stream->ModEventAsync(_poller,false,false);
    else
        SendToServer(params->method,params->controller,params->request);

}

void RpcChannelImpl::OnMessageSended(MessageStream* sream,Epoller* p){


}


}
