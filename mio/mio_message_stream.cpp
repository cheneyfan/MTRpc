#include "mio_message_stream.h"


namespace mtrpc {


int MessageStream::OnRecived(Epoller *p){


    int ret = header.ParserHeader(readbuf);

    if(ret == HTTP_PARSER_FAIL )
    {
        return -1;
    }

    if(ret == HTTP_PASER_HALF)
    {
        return 0;
    }

    // need read more
    if(header.content_length < readbuf.GetBufferUsed())
    {
        return 0;
    }




}


}
