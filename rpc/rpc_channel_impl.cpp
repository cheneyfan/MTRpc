#include "rpc_channel.h"
#include "rpc_channel_impl.h"
#include "mio/mio_error_code.h"
#include "mio/mio_connect_stream.h"
#include "rpc_controller_impl.h"
#include "log/log.h"

namespace mtrpc {

RpcChannelImpl::RpcChannelImpl(const RpcChannelOptions &options):
_options(options),_stream(NULL){

}

RpcChannelImpl::~RpcChannelImpl()
{

    if(_stream)
    {
         _stream->ReleaseRef();
    }
}

bool RpcChannelImpl::IsAddressValid(){
    return true;
}


RpcController* RpcChannelImpl:: GetController(){

    RpcController* cntl = new RpcController();
    return cntl;
}

int RpcChannelImpl::Connect(const std::string& server_ip,int32_t server_port){



    _stream = new ConnectStream();
    _stream->group = _group;
    _stream->RequireRef();

    _stream->handerWriteable =
            NewPermanentExtClosure(this,&RpcChannelImpl::OnWriteable);

    _stream->handerMessageRecived =
            NewPermanentExtClosure(this,&RpcChannelImpl::OnMessageRecived);
    _stream->handerMessageSended =
            NewPermanentExtClosure(this,&RpcChannelImpl::OnMessageSended);

    _stream->handerClose  =
            NewPermanentExtClosure(this,&RpcChannelImpl::OnClose);



   int ret = _stream->Connect(server_ip, server_port);

   if(ret == CONNECT_Ok)
   {
       _stream->AddEventASync(_poller,true,false);
       return 0;

   }else if(ret == CONNECT_ING){

       _stream->AddEventASync(_poller,true,true);
       _stream->Wait();

       return _stream->_ConnectStatus == CONNECT_Ok ? 0 : -1 ;
   }

   return -1;
}


void RpcChannelImpl::OnWriteable(SocketStream *sream, Epoller *p){

    if(sream->writebuf.isEmpty())
    {
        int size = 0;
        CallParams * call = NULL;
        {
             WriteLock<MutexLock> lc(pendinglock);
             size = pendingcall.size();
             if(size > 0)
                 call = pendingcall.front();
        }

        if(size ==0 )
            sream->ModEventAsync(_poller,true,false);
        else{
            SendToServer(call->method, call->controller, call->request);
        }
    }

}

void RpcChannelImpl::OnClose(SocketStream *sream, Epoller *p){

    WriteLock<MutexLock> lc(pendinglock);
    while(!pendingcall.empty()){
        CallParams * call = pendingcall.front();
        pendingcall.pop();
        delete call;
    }
}


void RpcChannelImpl::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                ::google::protobuf::RpcController* controller,
                const ::google::protobuf::Message* request,
                ::google::protobuf::Message* response,
                ::google::protobuf::Closure* done){


    if(_stream == NULL
            || _stream->_ConnectStatus == CONNECT_FAIL)
    {
        controller->SetFailed("Not Connect");
        done->Run();
        return ;
    }


    CallParams *currentCall = new CallParams();

    currentCall->method = method;
    currentCall->controller = controller;
    currentCall->request = request;
    currentCall->response = response;
    currentCall->done = done;
    pendingcall.push(currentCall);

    int size = 0;
    {
         WriteLock<MutexLock> lc(pendinglock);
         size = pendingcall.size();
    }

    if(size == 1 )
    {
        _stream->ModEventAsync(_poller,true,true);
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

    bool ret = request->SerializeToZeroCopyStream(&buf);

    TRACE("write ret:"<<ret
          <<" writebuf,w:"<<buf.writepos.toString()
          <<",r:"<<buf.readpos.toString());

    _stream->packetEnd = buf.writepos;

    int content_length = buf.writepos - mark;

    reqheader.SetContentLength(content_length);
    reqheader.SerializeRequestHeader(it);

    TRACE("header:"<<it.get()->buffer);

    return 0;
}

void RpcChannelImpl::OnMessageRecived(MessageStream* sream,Epoller* p){

    CallParams* params = NULL;

    {
         WriteLock<MutexLock> wl(pendinglock);
         params = pendingcall.front();
         pendingcall.pop();
    }

    if(params == NULL)
    {
        WARN("no method pending");
        return;
    }

    TRACE("parse begin:"
          <<"readbuf,w:"<<sream->readbuf.writepos.toString()
          <<",r:"<<sream->readbuf.readpos.toString()<<",status:"<<sream->reqheader.status);

    int ret = params->response->ParseFromZeroCopyStream(&sream->readbuf);

    RpcController * cntl = (RpcController *)params->controller;



    TRACE("parse end:"<<ret
          <<"readbuf,w:"<<sream->readbuf.writepos.toString()
          <<",r:"<<sream->readbuf.readpos.toString()<<",res:"<<params->response->DebugString());

        cntl->SetStatus(sream->reqheader.status);


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
        _stream->ModEventAsync(_poller,true,false);
    else
        SendToServer(params->method,params->controller,params->request);

}

void RpcChannelImpl::OnMessageSended(MessageStream* sream,Epoller* p){


}





}
