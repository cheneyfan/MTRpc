#include "mio_message_stream.h"
#include "mio/tcpsocket.h"
#include "log/log.h"

namespace mtrpc {


MessageStream::MessageStream(int sockfd):
    SocketStream(),
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




int MessageStream::OnRecived(Epoller *p, uint32_t buffer_size){

    //consume the buffer
    do{
        reqheader.ParserHeader(readbuf);

        if(ret == HTTP_PARSER_FAIL )
        {
            this->handerMessageError(this,p,HTTP_PARSER_FAIL);
            WARN(name<<"HTTP_PARSER_FAIL");
            return -1;
        }

        if(ret == HTTP_PASER_HALF)
        {
            return 0;
        }

        if(reqheader.GetContentLength() < 0)
        {
            this->handerMessageError(this,p,HTTP_REQ_NOLENGTH);
            return -1;
        }


        int body_size = readbuf.writepos - reqheader.bodyStart;

        TRACE(name<<"conteng length:"<<reqheader.GetContentLength()<<",recv body_size:"<<body_size);

        // need read more
        if(reqheader.GetContentLength() > body_size)
        {
            return 0;
        }

        IOBuffer::Iterator it = readbuf.readpos;

        //process packet
        if(handerMessageRecived)
        {
            handerMessageRecived->Run(this, p);
            reqheader.Reset();
            //begin next parser
        }

        if(this->_close_when_empty)
            break;

        buffer_size -= (readbuf.readpos - it);

    }while(buffer_size > 0);

    return 0;
}


int MessageStream::OnSended(Epoller *p, uint32_t buffer_size)
{

    //for
    handerMessageSended->Run(this,p,buffer_size);

    return 0;
}




}
