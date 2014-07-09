#include "mio_connect_stream.h"
#include "mio/tcpsocket.h"
#include "log/log.h"

namespace mtrpc {



ConnectStream::ConnectStream()
    :MessageStream()
{
    _fd  = TcpSocket::socket();

    TcpSocket::setNoblock(_fd,true);
    TcpSocket::setNoTcpDelay(_fd,true);

    _ConnectStatus = CONNECT_FAILE;
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
