#ifndef _MTPRC_RPC_CLIENT_IMPL_H_
#define _MTPRC_RPC_CLIENT_IMPL_H_

#include <google/protobuf/message.h>
#include "rpc_client.h"

namespace mtrpc {



class Epoller;
class WorkGroup;
class RpcChannel;

class RpcClientImpl
{


public:
    explicit RpcClientImpl(const RpcClientOptions& opt);

    virtual ~RpcClientImpl();

    void Start();

    void Stop();

    void Join();

    RpcClientOptions GetOptions();

    void ResetOptions(const RpcClientOptions& options);

    int ConnectionCount();

    RpcChannel* GetChannel(const std::string& server_addr,const RpcChannelOptions& ropt = RpcChannelOptions());

    void ReleaseChannel(RpcChannel* channel);


    static google::protobuf::LogHandler* ClientLogHander(
                           google::protobuf::LogLevel level,
                           const char* filename, int line,
                           const std::string& message);

public:
    RpcClientOptions _options;

private:

    Epoller* _poller;
    WorkGroup* _group;


}; // class RpcClientImpl
}

#endif

