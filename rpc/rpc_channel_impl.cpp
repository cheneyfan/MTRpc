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

    if(ret == CLIENT_CONNECT_IPROCESS)
    {
        _stream->AddEventASync(_poller,true,true);
        _stream->Wait();
    }

    if( stream->_ConnectStatus == CLIENT_CONNECT_OK)
    {
        _stream->AddEventASync(_poller,true,false);
        return 0;
    }

    // release
    _stream->ReleaseRef();
    _stream = NULL;

    return -1;
}


void RpcChannelImpl::OnWriteable(SocketStream *sream, Epoller *p){


    //once only send one packet
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

    //TODO reset sream
}


void RpcChannelImpl::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                                ::google::protobuf::RpcController* controller,
                                const ::google::protobuf::Message* request,
                                ::google::protobuf::Message* response,
                                ::google::protobuf::Closure* done){

    RpcClientController * cntl = (RpcClientController * )controller;

    if(_stream == NULL
            || _stream->_ConnectStatus == CLIENT_CONNECT_FAIL)
    {
        cntl->SetStatus(CLIENT_CONNECT_FAIL);
        if(done)
            done->Run();
        return ;
    }

    cntl->method = method;
    cntl->request = request;
    cntl->response = response;
    cntl->done = done;

    //invoid the call by event thread
    WriteLock<MutexLock> lc(pendinglock);
    if(!pendingcall.empty()){
        pendingcall.push(cntl);
        return;
    }

    SendToServer(method, controller, request);
    _stream->ModEventAsync(_poller,true,true);
}

int RpcChannelImpl::SendToServer(const ::google::protobuf::MethodDescriptor* method,
                                 RpcClientController* cntl,
                                 const ::google::protobuf::Message* request){

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

void RpcChannelImpl::OnMessageRecived(ConnectStream *sream, Epoller* p){

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


    //notify the call
    cntl->SetStatus(sream->reqheader.status);

    params->done->Run();

    delete  params;

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

void RpcChannelImpl::OnMessageSended(ConnectStream* sream,Epoller* p){

    TRACE("the packet has end,writebuf rpos:"<<sream->writebuf.readpos.toString()<<", wpos:"<<sream->writebuf.writepos.toString());

}





}
