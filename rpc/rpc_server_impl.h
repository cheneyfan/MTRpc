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

#include "google/protobuf/message.h"
namespace mtrpc{

class Epoller;
class WorkGroup;
class MessageStream;
class ServicePool;
class RpcController;

class RpcServerImpl
{
public:

    RpcServerImpl();
    RpcServerImpl(const RpcServerOptions& options);

    virtual ~RpcServerImpl();

    int Start(const std::string& server_address);

    int Join();
    void Stop();

    RpcServerOptions GetOptions();

    void ResetOptions(const RpcServerOptions& options);

    bool RegisterService(google::protobuf::Service* service, bool take_ownership=true);

    int ServiceCount();

    int ConnectionCount();

    std::string getHostIp();

public:

    void OnAccept(int sockfd);

    bool handlerGetServiceAndMethod(const std::string& method_full_name,
                                    google::protobuf::Service**service,
                                    google::protobuf::MethodDescriptor**method);

    bool ParseMethodFullName(const std::string& method_full_name,
            std::string &service_full_name, std::string &method_name);


private:
    Acceptor acceptor;
    RpcServerOptions _options;
    Epoller * poller;
    WorkGroup* group;
    ServicePool* _service_pool;



}; // class RpcServerImpl

} // namespace mtrpc


#endif // _SOFA_PBRPC_RPC_SERVER_IMPL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
