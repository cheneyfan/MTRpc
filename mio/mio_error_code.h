#ifndef _MTRPC_RPC_ERROR_CODE_H_
#define _MTRPC_RPC_ERROR_CODE_H_

namespace mtrpc{

enum ErrorCode {
    RPC_SUCCESS = 200,

    //for http parser
    HTTP_PARSER_FAIL  = 100,
    HTTP_PASER_HALF   = 101,
    HTTP_PASER_FINISH = 102,

    CONNECT_OK        = 1000,
    CONNECT_FAIL      = 1001,
    CONNECT_IPROCESS  = 10002,

};

// Convert rpc error code to human readable string.
const char* ErrorCodeToString(int error_code);


} // namespace mtrpc

#endif // _SOFA_PBRPC_RPC_ERROR_CODE_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
