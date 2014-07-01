#include "mio_error_code.h"

namespace mtrpc {

#define MAKE_CASE(name) case name: return (#name)

const char* ErrorCodeToString(int error_code)
{
    switch(error_code)
    {
        MAKE_CASE(RPC_SUCCESS);
    }
    return "ERROR_UNDEFINED";
}

} // namespace mtrpc
