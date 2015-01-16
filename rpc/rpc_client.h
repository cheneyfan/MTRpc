#ifndef _MTRPC_RPC_CLIENT_H_
#define _MTRPC_RPC_CLIENT_H_

#include <string>
#include "rpc_channel.h"
#include "rpc_controller.h"
namespace mtrpc {



class RpcChannel;

struct RpcClientOptions
{
    int work_thread_num;
    int send_timeout_sec;
    int recv_timeout_sec;

    RpcClientOptions()
        : work_thread_num(4)
        , send_timeout_sec(3)
        , recv_timeout_sec(3)
    {

    }
};


class RpcClientImpl;
class RpcClient
{
public:
    explicit RpcClient(const RpcClientOptions& options = RpcClientOptions());
    virtual ~RpcClient();

    // Get the current rpc client options.
    RpcClientOptions GetOptions();

    // Reset the rpc client options.
    //
    // Current only these options can be reset (others will be ignored):
    // * max_pending_buffer_size : will take effective immediately.
    // * max_throughput_in : will take effective from the next time slice (0.1s).
    // * max_throughput_out : will take effective from the next time slice (0.1s).
    //
    // Though you want to reset only part of these options, the other options also
    // need to be set.  Maybe you can reset by this way:
    //     RpcClientOptions options = rpc_client->GetOptions();
    //     options.max_throughput_in = new_max_throughput_in; // reset some options
    //     rpc_client->ResetOptions(options);
    //
    // The old and new value of reset options will be print to INFO log.
    void ResetOptions(const RpcClientOptions& options);

    // Get the count of current alive connections.
    int ConnectionCount();

    // Shutdown the rpc client.
    void Shutdown();

    void Join();

    RpcChannel* GetChannel(const std::string& server_addr, const RpcChannelOptions &opt=RpcChannelOptions());

    void ReleaseChannel(RpcChannel* channel);

private:
    RpcClientImpl* _impl;

}; // class RpcClient

}
#endif
