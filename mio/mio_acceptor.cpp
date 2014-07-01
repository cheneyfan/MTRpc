#include "log/log.h"
#include "tcpsocket.h"
#include "mio_acceptor.h"
#include "mio_poller.h"

namespace mtrpc {

Acceptor::Acceptor(){

    _fd = TcpSocket::socket();
    TcpSocket::setNoblock(_fd, true);
    TcpSocket::setNoTcpDelay(_fd, true);
}

virtual Acceptor::~Acceptor(){
    ::close(_fd);
}


void Acceptor::onEvent(Epoller* p,uint32_t events)
{

    struct sockaddr_in addr;
    socklen_t socksize = sizeof(sockaddr_in);

    int sockfd = TcpSocket::accept(_fd,(sockaddr*)&addr, socksize);

    if(sockfd < 0)
    {
        WARN("Accept a bad socket:"<<sockfd);
        return;
    }

    onAccept.Run(sockfd);

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


int Acceptor::StartListen(std::string service_addres){

    char host[64]= {0};
    int  port = 0;

    sscanf(service_addres.c_str(),"%[^:]:%d",host,&port);


    return StartListen(host,port);
}
}
