#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

#include "tcpsocket.h"
#include "log.h"


int TcpSocket::socket()
{
    return ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

int TcpSocket::connect(int sockfd,std::string ip,int port)
{
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr=inet_addr(ip.c_str());
    return ::connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
}

int TcpSocket::send(int sockfd, const char* buffer,int len)
{
    int send_len = 0;
    while(send_len < len)
    {
        int ret = ::send(sockfd,buffer+send_len,len-send_len,0);
        if(ret > 0)
            send_len += ret;
        else
        {
            WARN("sockfd:"<<sockfd<<",ret:"<<ret<<",errno:"<<errno<<",str:"<<strerror(errno));
            // for inter continue
            if(errno == EINTR)
            {
                WARN("read continue sockfd:"<<sockfd<<",ret:"<<ret<<",errno:"<<errno<<",str:"<<strerror(errno));
                continue;
            }

            return -1;
        }
    }
    return send_len == len ? len :-1;
}

int TcpSocket::recv(int sockfd, char* buffer,int len)
{
    int recv_size = 0;
    while(recv_size < len)
    {
        int ret = ::recv(sockfd,buffer+recv_size,len-recv_size,0);

        if(ret > 0)
            recv_size+= ret;

        else if(ret == 0)
        {
            WARN("sockfd:"<<sockfd<<",ret:"<<ret<<",errno:"<<errno<<",str:"<<strerror(errno));              return recv_size;
        }else {

            // for inter continue
            if(errno == EINTR)
            {
                WARN("recv continue sockfd:"<<sockfd<<",ret:"<<ret<<",errno:"<<errno<<",str:"<<strerror(errno));
                continue;
            }

            WARN("sockfd:"<<sockfd<<",ret:"<<ret<<",errno:"<<errno<<",str:"<<strerror(errno));
            return -1;
        }
    }

    return recv_size;
}

int TcpSocket::close(int sockfd){
    ::close(sockfd);
    return 0;
}

int TcpSocket::check(int sockfd)
{
    int ret = getSocketError(sockfd);
    if(0 != ret )
    {
         WARN("socket error ,sockfd:"<<sockfd<<",ret:"<<ret<<",errno:"<<errno<<",str:"<<strerror(errno));
         return -1;
    }

    char msg[1] = {0};
    ret = ::recv(sockfd, msg, sizeof(msg), MSG_DONTWAIT|MSG_PEEK);

    /* client already close connection. */
    if(ret == 0 || (ret < 0 && errno != EAGAIN))
    {
        WARN("socket closed ,sockfd:"<<sockfd<<",ret:"<<ret<<",errno:"<<errno<<",str:"<<strerror(errno));
        return -1;
    }

    return 0;
}

int TcpSocket::getpeer(int sockfd, char* ip ,int& port)
{
    struct sockaddr_in addr;
    int len = sizeof(sockaddr_in);

    if(0 !=::getpeername(sockfd,(struct sockaddr *)&addr,(socklen_t*)&len))
        return -1;

    port = ntohs(addr.sin_port);
    inet_ntop(AF_INET,(void*)&addr.sin_addr,ip,24);

    return 0;
}

int TcpSocket::getlocal(int sockfd, char* ip,int& port){

    struct sockaddr_in addr;
    int len = sizeof(addr);

    if(0 !=::getsockname(sockfd,(struct sockaddr *)&addr,(socklen_t*)&len))
        return -1;

    port=ntohs(addr.sin_port);
    inet_ntop(AF_INET,(void*)&addr.sin_addr,ip,24);

    return 0;
}

int TcpSocket::getSocketName(int sockfd,char* buffer,int size)
{

    char localhost[24]={"InVaid"};
    int localport = 0;

    TcpSocket::getlocal(sockfd, localhost,localport);

    char peerhost[24]={"InVaid"};
    int  peerport = 0;

    TcpSocket::getpeer(sockfd, peerhost,peerport);


    snprintf(buffer,size,"%d(%s:%d->%s:%d)", sockfd, localhost, localport, peerhost, peerport);

    return 0;
}

int TcpSocket::setNoblock(int sockfd, bool on){
    int flags = ::fcntl(sockfd, F_GETFL, 0);
    if(on){
        flags |= O_NONBLOCK;
    }else{
        flags &= ~O_NONBLOCK;
    }
    return ::fcntl(sockfd, F_SETFL, flags);
}

int TcpSocket::setNoTcpDelay(int sockfd, bool on){

    int optval = on ? 1 : 0;
    return ::setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY,
                        &optval, static_cast<socklen_t>(sizeof optval));
}

int TcpSocket::setKeepAlive(int sockfd, bool on){

    int optval = on ? 1 : 0;
    ::setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof optval));
    int idle = 60;
    if(setsockopt (sockfd, SOL_TCP, TCP_KEEPIDLE, &idle, sizeof idle) != 0)
    {
        return -1;
    }

    int intv = 5;
    if(setsockopt (sockfd, SOL_TCP, TCP_KEEPINTVL, &intv, sizeof intv) != 0)
    {
        return -1;
    }


    int cnt = 3;
    if (setsockopt (sockfd, SOL_TCP, TCP_KEEPCNT, &cnt, sizeof cnt) != 0)
    {

        return -1;
    }
    return 0;
}

int TcpSocket::setNoLinger(int sockfd, bool on){

    linger ling;
    ling.l_onoff = (int)on;
    ling.l_linger = 0;
    return ::setsockopt(sockfd,
                        SOL_SOCKET,
                        SO_LINGER,
                        (const char*)&ling,
                        sizeof(linger));
}

int TcpSocket::setReUseAddr(int sockfd, bool on){

    int optval = on ? 1 : 0;
    return ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                        &optval, static_cast<socklen_t>(sizeof optval));

}



int TcpSocket::setSendTimeOut(int sockfd, int time_ms){

    struct timeval timeout;

    timeout.tv_sec  = time_ms / 1000;
    timeout.tv_usec = (time_ms%1000)*1000;

    return ::setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(struct timeval));
}

int TcpSocket::setRecvTimeOut(int sockfd, int time_ms){

    struct timeval timeout;
    timeout.tv_sec =  time_ms / 1000;
    timeout.tv_usec = (time_ms%1000)*1000;
    return ::setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));
}


ssize_t TcpSocket::readv(int fd, const struct iovec *iov, int iovcnt){

    return ::readv(fd,iov,iovcnt);
}


int TcpSocket::bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen){

    return ::bind(sockfd,addr,addrlen);
}


int TcpSocket::bind(int sockfd,std::string ip, int port){

    struct sockaddr_in addr;

    if(fromIpPort(ip.c_str(), port, &addr) != 0)
        return -1;

    return ::bind(sockfd, (struct sockaddr *)&addr, sizeof (sockaddr_in));
}


int TcpSocket::listen(int sockfd)
{
    return ::listen(sockfd, 10);
}

int TcpSocket::accept(int sockfd,struct sockaddr *addr,socklen_t& addrlen){

    return ::accept(sockfd, addr,&addrlen);
}


int TcpSocket::fromIpPort(const char* ip, int port,
                      struct sockaddr_in* addr)
{
    memset(addr,0,sizeof(sockaddr_in));
    addr->sin_family = AF_INET;

    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = inet_addr(ip);

    return 0;
}


int TcpSocket::getSocketError(int sockfd)
{
    int optval = 0;
    socklen_t optlen = static_cast<socklen_t>(sizeof optval);

    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
    {
        return errno;
    }
    else
    {
        return optval;
    }
}
