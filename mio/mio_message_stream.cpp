#include "mio_message_stream.h"
#include "mio/tcpsocket.h"
#include "log/log.h"

namespace mtrpc {


MessageStream::MessageStream():
    SocketStream(),
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

    TRACE("recived:"<<readbuf.readpos.get()->buffer);

    TRACE("recived begin:"
          <<",length:"<<reqheader.content_length
          <<",read:"<<readbuf.GetBufferUsed()
          <<",rpos:"<<readbuf.readpos.toString()
          <<",wpos:"<<readbuf.writepos.toString());

    // if the header parsered

    if(!reqheader.isReqParsed() )
    {
        int ret = reqheader.ParserRequestHeader(readbuf);

        if(ret == HTTP_PARSER_FAIL )
        {
            return -1;
        }

        if(ret == HTTP_PASER_HALF)
        {
            return 0;
        }

        if( reqheader.GetContentLength() < 0)
        {
            return -1;
        }

        TRACE("recived:"<<ret
              <<",length:"<<reqheader.content_length
              <<",read:"<<readbuf.GetBufferUsed()
              <<",rpos:"<<readbuf.readpos.toString()
              <<",wpos:"<<readbuf.writepos.toString());


    }

    int body_size = readbuf.writepos - reqheader.bodyStart;

        TRACE("conteng length:"<<reqheader.GetContentLength()<<",recv body_size:"<<body_size);

    // need read more
    if(reqheader.GetContentLength() > body_size)
    {
        return 0;
    }

    //process packet
    if(handerMessageRecived)
    {
        handerMessageRecived->Run(this, p);
        reqheader.Reset();
        //begin next parser
    }

    return 0;
}


int MessageStream::OnSended(Epoller *p)
{

    if(packetEnd == writebuf.readpos && handerMessageSended)
    {
        handerMessageSended->Run(this,p);
        resheader.Reset();
    }

    return 0;
}




}
