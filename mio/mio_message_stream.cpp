#include "mio_message_stream.h"
#include "mio/tcpsocket.h"

namespace mtrpc {


MessageStream::MessageStream():
    handerMessageRecived(NULL),
    handerMessageSended(NULL)
{
}

MessageStream::MessageStream(int sockfd):
    handerMessageRecived(NULL),
    handerMessageSended(NULL)
{
    _fd  = sockfd;
    TcpSocket::setNoblock(_fd,true);
    TcpSocket::setNoTcpDelay(_fd,true);
}

MessageStream::~MessageStream(){

    delete handerMessageRecived;
    delete handerMessageSended;
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

    if(handerMessageRecived)
        handerMessageRecived->Run(this, p);

    return 0;
}


int MessageStream::OnSended(Epoller *p)
{

    if(packetEnd == writebuf.readpos && handerMessageSended)
    {
        handerMessageSended->Run(this,p);
    }

    return 0;
}




}
