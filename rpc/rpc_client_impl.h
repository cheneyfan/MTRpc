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
    explicit RpcClientImpl(const RpcClientOptions& options);

    virtual ~RpcClientImpl();

    void Start();

    void Stop();

    RpcClientOptions GetOptions();

    void ResetOptions(const RpcClientOptions& options);

    int ConnectionCount();

    // Rpc call method to remote endpoint.
    //
    // The call can be done in following cases:
    // * send failed
    // * timeouted
    // * response received
    void CallMethod(const google::protobuf::Message* request,
            google::protobuf::Message* response,
            const RpcControllerImpl*& cntl);

 private:

    Epoller* poller;
    WorkGroup* group;

}; // class RpcClientImpl
}

#endif

