#include "mio_message_stream.h"
#include "mio/tcpsocket.h"

namespace mtrpc {


MessageStream::MessageStream(){

}

MessageStream::MessageStream(int sockfd)
{
    _fd  = sockfd;
    TcpSocket::setNoblock(_fd,true);
    TcpSocket::setNoTcpDelay(_fd,true);
}


int MessageStream::OnRecived(Epoller *p){


    int ret = reqheader.ParserRequestHeader(readbuf);

    if(ret == HTTP_PARSER_FAIL )
    {
        return -1;
    }

    if(ret == HTTP_PASER_HALF)
    {
        return 0;
    }

    // need read more
    if(reqheader.content_length  >  readbuf.GetBufferUsed())
    {
        return 0;
    }

    // a
    handerMessageRecived->Run(this, p);

    return 0;
}


int MessageStream::OnSended(Epoller *p)
{

    if(packetEnd == writebuf.readpos)
    {
        handerMessageSended->Run(this,p);
    }

    return 0;
}

int MessageStream::OnConnect(Epoller *p){

    handerConnected->Run(this,p);

    return 0;
}
}
