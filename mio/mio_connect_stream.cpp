#include "mio_connect_stream.h"
#include "mio/tcpsocket.h"

namespace mtrpc {



ConnectStream::ConnectStream():
_ConnectStatus(CONNECT_FAILE)
{
    _fd  = TcpSocket::socket();
    TcpSocket::setNoblock(_fd,true);
    TcpSocket::setNoTcpDelay(_fd,true);
}


int ConnectStream::Connect(const std::string& server_ip,int32_t server_port){

    _server_ip = server_ip;
    _server_port = server_port;

    int ret = TcpSocket::connect(_fd, server_ip, server_port);
    int save_errno = errno;

    if(ret == 0)
    {
        _ConnectStatus = CONNECT_Ok;
        return 0;

    }else if(ret < 0 && save_errno != EINPROGRESS ){

        _ConnectStatus = CONNECT_FAILE;
        return -1;
    }

    _ConnectStatus = CONNECT_ING;

    return EINPROGRESS;
}

int ConnectStream::OnConnect(Epoller* p){

    // base
    SocketStream::OnConnect(p);

    _ConnectStatus = CONNECT_Ok;

    _stream->AddEventASync(_p,false,false);
    return 0;
}

}
