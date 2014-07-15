#include "mio_error_code.h"

namespace mtrpc {

#define MAKE_CASE(name) case name: return (#name)

const char* ErrorCodeToString(int error_code)
{
    switch(error_code)
    {
        MAKE_CASE(OK);
        MAKE_CASE(HTTP_PARSER_FAIL);
        MAKE_CASE(HTTP_PASER_HALF);
        MAKE_CASE(HTTP_PASER_FINISH);
        MAKE_CASE(HTTP_REQ_OVERSIZE);
        MAKE_CASE(HTTP_REQ_NOLENGTH);
        MAKE_CASE(HTTP_SERVER_NOTFOUND);
        MAKE_CASE(HTTP_METHOD_NOTFOUND);
        MAKE_CASE(SERVER_READBUFFER_EMPTY);
        MAKE_CASE(SERVER_READBUFFER_FULL);
        MAKE_CASE(SERVER_WRIEBUFFER_EMPTY);
        MAKE_CASE(SERVER_WRIEBUFFER_FULL);
        MAKE_CASE(SERVER_REQ_PARSER_FAILED);
        MAKE_CASE(SERVER_PROCESS_FAILED);
        MAKE_CASE(SERVER_RES_SERIA_FAILED);
        MAKE_CASE(CLIENT_CONNECT_OK);
        MAKE_CASE(CLIENT_CONNECT_FAIL);
        MAKE_CASE(CLIENT_CONNECT_IPROCESS);
    }

    return "ERROR_UNDEFINED";
}

} // namespace mtrpc
