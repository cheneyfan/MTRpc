#include "mio_connect_stream.h"
#include "mio/tcpsocket.h"
#include "log/log.h"

namespace mtrpc {



ConnectStream::ConnectStream()
    :SocketStream(),
      handerMessageRecived(NULL),
      handerMessageSended(NULL)
{
    _fd  = TcpSocket::socket();

    TcpSocket::setNoblock(_fd,true);
    TcpSocket::setNoTcpDelay(_fd,true);

    _ConnectStatus = CONNECT_FAILE;
}

ConnectStream::~ConnectStream()
{
    delete handerMessageRecived;
    delete handerMessageSended;
}

int ConnectStream::Connect(const std::string& server_ip,int32_t server_port){

    int ret = TcpSocket::connect(_fd, server_ip, server_port);

    int save_errno = errno;

    TRACE("sock:"<<_fd<<",connect "<<server_ip<<":"<<server_port<<",ret:"<<ret<<",errno:"<<errno<<","<<strerror(errno));

    if(ret == 0)
    {
        _ConnectStatus = CONNECT_Ok;
    }else if(ret < 0 && save_errno != EINPROGRESS )
    {
         _ConnectStatus = CONNECT_FAILE;
    }else
         _ConnectStatus = CONNECT_ING;

    return _ConnectStatus;
}


int ConnectStream::OnConnect(Epoller* p){

    // base
    SocketStream::OnConnect(p);


    {
        WriteLock<MutexLock> lock(mutex);
        _ConnectStatus = CONNECT_Ok;
    }
    cv.notifyOne();

    return 0;
}


int ConnectStream::OnRecived(Epoller *p){

    TRACE("recived:"<<readbuf.readpos.get()->buffer);

    TRACE("recived begin:"
          <<",length:"<<reqheader.content_length
          <<",read:"<<readbuf.GetBufferUsed()
          <<",rpos:"<<readbuf.readpos.toString()
          <<",wpos:"<<readbuf.writepos.toString());


    if(!resheader.isResParsed())
    {
        int ret = resheader.ParserReponseHeader(readbuf);

        if(ret == HTTP_PARSER_FAIL )
        {
            return -1;
        }

        if(ret == HTTP_PASER_HALF)
        {
            return 0;
        }

        if( resheader.GetContentLength() < 0)
        {
            return -1;
        }

        TRACE("recived:"<<ret
              <<",length:"<<resheader.content_length
              <<",read:"<<readbuf.GetBufferUsed()
              <<",rpos:"<<readbuf.readpos.toString()
              <<",wpos:"<<readbuf.writepos.toString());
    }



    int body_size = readbuf.writepos - resheader.bodyStart;

    TRACE("conteng length:"<<resheader.GetContentLength()<<",recv body_size:"<<body_size);
    // need read more
    if(resheader.GetContentLength()  >  body_size)
    {
        return 0;
    }

    if(handerMessageRecived)
    {
        //process one packet
        handerMessageRecived->Run(this, p);
        resheader.Reset();
        //begin next parser
    }

    return 0;
}


int ConnectStream::OnSended(Epoller *p)
{

    if(packetEnd == writebuf.readpos && handerMessageSended)
    {
        handerMessageSended->Run(this,p);
        reqheader.Reset();
    }

    return 0;
}


int ConnectStream::OnClose(Epoller* p){

    TRACE(name<<",closed");

    SocketStream::OnClose(p);


    WriteLock<MutexLock> lock(mutex);
    _ConnectStatus = CONNECT_FAILE;

    cv.notifyOne();

    return 0;
}


void ConnectStream::Wait(){

    while(_ConnectStatus == CONNECT_ING)
    {
        WriteLock<MutexLock> lock(mutex);
        cv.wait(&mutex._mutex);
    }
}

}
