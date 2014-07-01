// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#ifndef _SOFA_PBRPC_RPC_SERVER_IMPL_H_
#define _SOFA_PBRPC_RPC_SERVER_IMPL_H_

#include <deque>
#include "rpc_server.h"


namespace mtrpc{

class RpcServerImpl
{
public:
    static const int MAINTAIN_INTERVAL_IN_MS = 100;

public:
    RpcServerImpl(const RpcServerOptions& options,
                  RpcServer::EventHandler* handler);

    virtual ~RpcServerImpl();

    bool Start(const std::string& server_address);

    void Stop();

    RpcServerOptions GetOptions();

    void ResetOptions(const RpcServerOptions& options);

    bool RegisterService(google::protobuf::Service* service, bool take_ownership);

    int ServiceCount();

    int ConnectionCount();

    void GetPendingStat(int64* pending_message_count,
            int64* pending_buffer_size, int64* pending_data_size);

    bool IsListening();

    // Restart the listener.  It will restart listening anyway, even if it is already in
    // listening.  Return false if the server is not started, or fail to restart listening.
    bool RestartListen();

private:

    void OnCreated(const RpcServerStreamPtr& stream);

    void OnAccepted(const RpcServerStreamPtr& stream);

    void OnAcceptFailed(
            RpcErrorCode error_code,
            const std::string& error_text);

    void OnReceived(
            const RpcEndpoint& local_endpoint,
            const RpcEndpoint& remote_endpoint,
            const RpcMeta& meta,
            const RpcServerStreamWPtr& stream,
            const ReadBufferPtr& buffer,
            int64 data_size);

    static void OnCallMethodDone(
            RpcController* controller,
            google::protobuf::Message* request,
            google::protobuf::Message* response,
            MethodBoard* method_board,
            PTime start_time);

    static void SendFailedResponse(
            const RpcServerStreamWPtr& stream,
            uint64 sequence_id,
            int32 error_code,
            const std::string& reason);

    static void SendSucceedResponse(
            const RpcServerStreamWPtr& stream,
            uint64 sequence_id,
            CompressType compress_type,
            google::protobuf::Message* response);

    static void OnSendResponseDone(
            const RpcEndpoint& remote_endpoint,
            uint64 sequence_id,
            RpcErrorCode error_code);

    void StopStreams();

    void ClearStreams();

    void TimerMaintain(const PTime& now);

    static bool ParseMethodFullName(const std::string& method_full_name,
            std::string* service_full_name, std::string* method_name);

private:
    struct FlowControlItem
    {
        int token; // always <= 0
        RpcServerStream* stream;

        FlowControlItem(int t, RpcServerStream* s) : token(t), stream(s) {}
        // comparator: nearer to zero, higher priority
        bool operator< (const FlowControlItem& o) const
        {
            return token > o.token;
        }
    };

private:
    RpcServerOptions _options;

}; // class RpcServerImpl

} // namespace mtrpc


#endif // _SOFA_PBRPC_RPC_SERVER_IMPL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
