#include "log/log.h"
#include "tcpsocket.h"
#include "mio_acceptor.h"
#include "mio_poller.h"

namespace mtrpc {

Acceptor::Acceptor():
    IOEvent(),
    handerAccept(NULL)
{

    _fd = TcpSocket::socket();
    TcpSocket::setNoblock(_fd, true);
    TcpSocket::setNoTcpDelay(_fd, true);
}

Acceptor::~Acceptor(){
    ::close(_fd);
    delete handerAccept;
}


void Acceptor::OnEvent(Epoller* p,uint32_t events)
{

    struct sockaddr_in addr;
    socklen_t socksize = sizeof(sockaddr_in);

    int sockfd =0;
    do{
        sockfd = TcpSocket::accept(_fd, (sockaddr*)&addr, socksize);

        if(sockfd < 0)
        {
            WARN("Accept a bad socket:"<<sockfd<<",errno:"<<errno<<","<<strerror(errno));
            return;
        }

        TcpSocket::setNoblock(sockfd, true);
        TcpSocket::setNoTcpDelay(sockfd, true);

        handerAccept->Run(sockfd);
        std::string ip;
        int port = 0;
        TcpSocket::getpeer(sockfd,ip,port);
        TRACE("accept a new connection: sockfd:"<<sockfd<<",ip:"<<ip<<port);

    }while(sockfd >= 0);

}

int Acceptor::StartListen(const char* host,int port){

    TcpSocket::setReUseAddr(_fd, true);

    if(host ==NULL)
        host = "0.0.0.0";

    if(TcpSocket::bind(_fd, host, port) != 0)
    {
        WARN("bind "<<host<<":"<<port<<" failed");
        return -1;
    }

    if(TcpSocket::listen(_fd) != 0)
    {
        WARN("listen "<<host<<":"<<port<<" failed");
        return -1;
    }

    INFO("start listen,host:"<<host<<",port:"<<port<<",fd:"<<_fd);

    return 0;
}


int Acceptor::StartListen(const std::string &service_addres){

    char host[64]= {0};
    int  port = 0;

    sscanf(service_addres.c_str(),"%[^:]:%d",host,&port);

    return StartListen(host,port);
}
}
