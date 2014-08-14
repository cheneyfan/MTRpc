#include "mio_connect_stream.h"
#include "mio/tcpsocket.h"
#include "log/log.h"
#include "mio/mio_error_code.h"

namespace mtrpc {



ConnectStream::ConnectStream()
    :SocketStream(),
      handerMessageRecived(NULL),
      handerMessageSended(NULL)
{
    _fd  = TcpSocket::socket();

    TcpSocket::setNoblock(_fd,true);
    TcpSocket::setNoTcpDelay(_fd,true);
    TcpSocket::setKeepAlive(_fd,true);
    
    _ConnectStatus = CLIENT_CONNECT_FAIL;
}

ConnectStream::~ConnectStream()
{


    close(_fd);
    delete handerMessageRecived;
    delete handerMessageSended;

    handerMessageRecived = NULL;
    handerMessageSended  = NULL;
}

int ConnectStream::Connect(const std::string& server_ip,int32_t server_port){

    int ret = TcpSocket::connect(_fd, server_ip, server_port);

    int save_errno = errno;

    TRACE("sock:"<<_fd<<",connect "<<server_ip<<":"<<server_port<<",ret:"<<ret<<",errno:"<<errno<<","<<strerror(errno));

    if(ret == 0)
    {
        _ConnectStatus = CLIENT_CONNECT_OK;

    }else if(ret < 0 && save_errno != EINPROGRESS )
    {
         _ConnectStatus = CLIENT_CONNECT_FAIL;
    }else
         _ConnectStatus = CLIENT_CONNECT_IPROCESS;

    return _ConnectStatus;
}


int ConnectStream::OnConnect(Epoller* p){

    // base
    SocketStream::OnConnect(p);

    if(CLIENT_CONNECT_IPROCESS == _ConnectStatus)
    {
        WriteLock<MutexLock> lock(mutex);
        _ConnectStatus = CLIENT_CONNECT_OK;
        cv.notifyOne();
    }

    return 0;
}


int ConnectStream::OnRecived(Epoller *p, int32_t buffer_size){

    //consume the buffer
    do{
       // TRACE(GetSockName()<<",recv:"<<(readbuf.readpos.get()->buffer+readbuf.readpos._pos)<<",buffer:"<<buffer_size);

        IOBuffer::Iterator begin_parser = readbuf.readpos;
        int ret = resheader.ParserHeader(readbuf);
        buffer_size  -= (readbuf.readpos - begin_parser);


        if(ret == HTTP_PARSER_FAIL )
        {
            this->handerMessageError->Run(this,p,HTTP_PARSER_FAIL);
            WARN(GetSockName()<<"HTTP_PARSER_FAIL");
            return -1;
        }

        if(ret == HTTP_PASER_HALF)
        {
            return 0;
        }

        if(resheader.GetContentLength() < 0)
        {
            this->handerMessageError->Run(this,p,HTTP_REQ_NOLENGTH);
            return -1;
        }

        int body_size = readbuf.writepos - resheader.bodyStart;

        TRACE(GetSockName()<<"conteng length:"<<resheader.GetContentLength()<<",recv body_size:"<<body_size);

        // need read more
        if(resheader.GetContentLength() > body_size)
        {
            return 0;
        }

        IOBuffer::Iterator beginpacket = readbuf.readpos;

        //process packet
        if(handerMessageRecived)
        {
            handerMessageRecived->Run(this, p,buffer_size);

            resheader.Reset();
            readbuf.Reset();
            //begin next parser
        }

        TRACE("beginpacket:"<<beginpacket.toString()<<",end:"<<readbuf.readpos.toString());

        buffer_size = buffer_size -(readbuf.readpos - beginpacket);

        if(this->_close_when_empty)
            break;


    }while(buffer_size > 0);

    return 0;
}


int ConnectStream::OnSended(Epoller *p, int32_t buffer_size)
{

    if(handerMessageSended)
        handerMessageSended->Run(this,p,buffer_size);
    return 0;
}


int ConnectStream::OnClose(Epoller* p){

    TRACE(GetSockName()<<",closed");

    SocketStream::OnClose(p);

    if(CLIENT_CONNECT_IPROCESS ==_ConnectStatus)
    {
        WriteLock<MutexLock> lock(mutex);
        _ConnectStatus = CLIENT_CONNECT_FAIL;
        cv.notifyOne();
    }

    return 0;
}


void ConnectStream::Wait(){

    while(_ConnectStatus == CLIENT_CONNECT_IPROCESS)
    {
        WriteLock<MutexLock> lock(mutex);
        cv.wait(&mutex._mutex);
    }
}

}
