// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#ifndef _MTRPC_RPC_SERVER_H_
#define _MTRPC_RPC_SERVER_H_

#include <google/protobuf/service.h>


namespace mtrpc {


// Defined in other files.
class RpcServerImpl;

struct RpcServerOptions {
    int work_thread_num;

    RpcServerOptions()
        : work_thread_num(1)
    {}
};

///
/// \brief The RpcServer class
///
class RpcServer
{
public:
    // Constructor.
    // @param options The rpc server options.
    // @param handler The event handler.  It will be taken overby the rpc server and will be
    //                deleted when the server destroys.
    explicit RpcServer(const RpcServerOptions& options = RpcServerOptions());

    virtual ~RpcServer();


    bool Start(const std::string& server_address);


    void Stop();


    int Join();


    RpcServerOptions GetOptions();


    void ResetOptions(const RpcServerOptions& options);


    bool RegisterService(google::protobuf::Service* service, bool take_ownership = true);

    // Get the count of current registed services.
    int ServiceCount();

    // Get the count of current alive connections.
    int ConnectionCount();



public:
    const RpcServerImpl* impl() const
    {
        return _impl;
    }
private:
    RpcServerImpl* _impl;

}; // class RpcServer

} // namespace mtrpc

#endif // _SOFA_PBRPC_RPC_SERVER_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
