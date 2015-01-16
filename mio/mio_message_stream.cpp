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
    TRACE("MessageStream create:"<<_fd);
}

MessageStream::~MessageStream(){

    TRACE("MessageStream close:"<<_fd);
    close(_fd);
    delete handerMessageRecived;
    delete handerMessageSended;
}




int MessageStream::OnRecived(Epoller *p, int32_t buffer_size){

    //consume the buffer
    do{

      TRACE(GetSockName()<<",recv,read:"<<readbuf.readpos.toString()<<",write:"<<readbuf.writepos.toString()<<",buffer_size:"<<buffer_size);

        IOBuffer::Iterator begin_parser = readbuf.readpos;
        int ret = reqheader.ParserHeader(readbuf);

        TRACE(GetSockName()<<"ParserHeader,ret:"<<ret<<", state:"<<reqheader.state<<",read:"<<readbuf.readpos.toString()<<",write:"<<readbuf.writepos.toString()<<",buffer_size:"<<buffer_size);

        buffer_size -= (readbuf.readpos - begin_parser);


        if(ret == HTTP_PARSER_FAIL )
        {
            this->handerMessageError->Run(this, p, HTTP_PARSER_FAIL);
            WARN(GetSockName()<<"HTTP_PARSER_FAIL,state:"<<reqheader.state);
            return -1;
        }

        if(ret == HTTP_PASER_HALF)
        {
            WARN(GetSockName()<<"HTTP_PASER_HALF,state:"<<reqheader.state);
            return 0;
        }
        
        int32_t max_length = DEFAULT_BUFFER_SIZE * MAX_BUFFER_PIECES;
        TRACE("MessageStream::OnRecived, max_length is "<< max_length);
        if (reqheader.GetContentLength() > max_length) {
            this->handerMessageError->Run(this, p, HTTP_REQUEST_OVERFLOW);
            WARN(GetSockName()<<"HTTP_REQUEST_OVERFLOW, max_length is :"<<max_length); 
            return -1; 
        }
        /*
        if(reqheader.GetContentLength() < 0)
        {
            this->handerMessageError->Run(this,p,HTTP_REQ_NOLENGTH);
            return -1;
        }*/


        int body_size = readbuf.writepos - reqheader.bodyStart -1;


        TRACE(GetSockName()<<"conteng length:"<<reqheader.GetContentLength()<<",recv body_size:"<<body_size);

        // need read more
        if(reqheader.GetContentLength() > body_size)
        {
            return 0;
        }


        IOBuffer::Iterator beginpacket = readbuf.readpos;

        //process packet
        if(handerMessageRecived)
        {
            handerMessageRecived->Run(this, p, buffer_size);

            reqheader.Reset();
            readbuf.Reset();
            //begin next parser
        }

        buffer_size = buffer_size -(readbuf.readpos - beginpacket);

        if(this->_close_when_empty)
        {
            break;
        }

    }while(buffer_size > 0);

    return 0;
}


int MessageStream::OnSended(Epoller *p, int32_t buffer_size)
{

    //for
    handerMessageSended->Run(this,p,buffer_size);

    return 0;
}




}
