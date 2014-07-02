// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#ifndef _SOFA_PBRPC_RPC_SERVER_IMPL_H_
#define _SOFA_PBRPC_RPC_SERVER_IMPL_H_

#include <deque>
#include "rpc_server.h"
#include "mio/mio_acceptor.h"

namespace mtrpc{

class EPoller;
class WorkGroup;
class MessageStream;
class ServicePool;

class RpcServerImpl
{
public:
    static const int MAINTAIN_INTERVAL_IN_MS = 100;

public:
    RpcServerImpl(const RpcServerOptions& options);

    virtual ~RpcServerImpl();

    bool Start(const std::string& server_address);

    void Stop();

    RpcServerOptions GetOptions();

    void ResetOptions(const RpcServerOptions& options);

    bool RegisterService(google::protobuf::Service* service, bool take_ownership);

    int ServiceCount();

    int ConnectionCount();


public:

    void OnAccept(int sockfd);
    int  OnMessageRecived(MessageStream* stream);


private:

    EPoller * poller;
    WorkGroup* group;
    Acceptor acceptor;
private:
    RpcServerOptions _options;
    ServicePool* pool;

}; // class RpcServerImpl

} // namespace mtrpc


#endif // _SOFA_PBRPC_RPC_SERVER_IMPL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
