#ifndef _MRRPC_RPC_CONTROLLER_IMPLE_H_
#define _MRRPC_RPC_CONTROLLER_IMPLE_H_

namespace mtrpc {

#define CompressTypeAuto ((CompressType)-1)

class RpcControllerImpl : public sofa::pbrpc::enable_shared_from_this<RpcControllerImpl>
{
public:
    typedef boost::function<void(const RpcControllerImplPtr&)> InternalDoneCallback;

public:
    RpcControllerImpl()
        : _sequence_id(0)
        , _error_code(RPC_SUCCESS)
        , _is_done(NOT_DONE)
        , _is_request_sent(false)
        , _sent_bytes(0)
        , _is_start_cancel(false)
        , _is_sync(false)
        , _timeout_id(0)
    {}

}


}
#endif
