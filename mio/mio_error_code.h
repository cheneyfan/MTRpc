#ifndef _MTRPC_RPC_ERROR_CODE_H_
#define _MTRPC_RPC_ERROR_CODE_H_

namespace mtrpc{

enum ErrorCode {
    SUCCESS = 0,

    //for http parser
    HTTP_PARSER_FAIL  = 100,
    HTTP_PASER_HALF   = 101,
    HTTP_PASER_FINISH = 102,

};

// Convert rpc error code to human readable string.
const char* ErrorCodeToString(int error_code);


} // namespace mtrpc

#endif // _SOFA_PBRPC_RPC_ERROR_CODE_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
