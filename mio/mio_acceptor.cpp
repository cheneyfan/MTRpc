#include "log/log.h"
#include "tcpsocket.h"
#include "mio_acceptor.h"
#include "mio_poller.h"

namespace  MTRpc {

Acceptor::Acceptor(){
    ev._fd = TcpSocket::socket();
    ev.ev.data.ptr = &ev;

    TcpSocket::setNoblock(ev._fd, true);
    TcpSocket::setNoTcpDelay(ev._fd, true);

    ev.onEvent = Closure::From<Acceptor,Epoller*,uint32_t,&Acceptor::onEvent>(this);
}

Acceptor::~Acceptor(){
    ::close(ev._fd);
}


void Acceptor::onEvent(Epoller* p,uint32_t events)
{

    struct sockaddr_in addr;
    socklen_t socksize = sizeof(sockaddr_in);

    int sockfd = TcpSocket::accept(ev._fd,(sockaddr*)&addr, socksize);

    if(sockfd < 0)
    {
        WARN("Accept a bad socket:"<<sockfd);
        return;
    }

    onAccept(sockfd);

    TRACE_FMG("accept a new connection: sockfd:%u",sockfd);
}

int Acceptor::StartListen(const char* host,int port){

    TcpSocket::setReUseAddr(ev._fd, true);

    if(host ==NULL)
        host = "0.0.0.0";

    if(TcpSocket::bind(ev._fd, host, port) != 0)
    {
        WARN("bind "<<host<<":"<<port<<" failed");
        return -1;
    }

    if(TcpSocket::listen(ev._fd) != 0)
    {
        WARN("listen "<<host<<":"<<port<<" failed");
        return -1;
    }

    return 0;
}

}
