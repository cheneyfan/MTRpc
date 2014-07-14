#ifndef _MTRPC_RPC_ERROR_CODE_H_
#define _MTRPC_RPC_ERROR_CODE_H_

namespace mtrpc{

enum ErrorCode {
    OK = 200,

    //for http parser
    HTTP_PARSER_FAIL  = 400,
    HTTP_PASER_HALF   = 401,
    HTTP_PASER_FINISH = 402,

    HTTP_REQ_OVERSIZE = 410,
    HTTP_REQ_NOLENGTH = 411,


    HTTP_SERVER_NOTFOUND = 423,
    HTTP_METHOD_NOTFOUND = 424,



    SERVER_READBUFFER_EMPTY  = 500,
    SERVER_READBUFFER_FULL   = 501,

    SERVER_WRIEBUFFER_EMPTY  = 502,
    SERVER_WRIEBUFFER_FULL   = 503,

    SERVER_REQ_PARSER_FAILED = 504,
    SERVER_PROCESS_FAILED    = 505,
    SERVER_RES_SERIA_FAILED  = 506,



    CLIENT_CONNECT_OK        = 1000,
    CLIENT_CONNECT_FAIL      = 1001,
    CLIENT_CONNECT_IPROCESS  = 10002,

};

// Convert rpc error code to human readable string.
const char* ErrorString(int error_code);


} // namespace mtrpc

#endif // _SOFA_PBRPC_RPC_ERROR_CODE_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
