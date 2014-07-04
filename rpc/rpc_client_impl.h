#ifndef _MTPRC_RPC_CLIENT_IMPL_H_
#define _MTPRC_RPC_CLIENT_IMPL_H_

#include "rpc_client.h"
#include <google/protobuf/message.h>

namespace mtrpc {


class RpcControllerImpl;
class Epoller;
class WorkGroup;

class RpcClientImpl
{


public:
    explicit RpcClientImpl(const RpcClientOptions& opt);

    virtual ~RpcClientImpl();

    void Start();

    void Stop();

    RpcClientOptions GetOptions();

    void ResetOptions(const RpcClientOptions& options);

    int ConnectionCount();



public:
    RpcClientOptions _options;

private:

    Epoller* poller;
    WorkGroup* group;


}; // class RpcClientImpl
}

#endif

