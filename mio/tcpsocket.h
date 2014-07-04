#ifndef _TCP_SOCKET_H_
#define _TCP_SOCKET_H_

#include <string>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>

///
/// \brief The TcpSocket class
///
class TcpSocket
{
public:

    ///
    /// \brief socket
    /// \return
    ///
    static int socket();

    ///
    /// \brief connect
    /// \param sockfd
    /// \param ip
    /// \param port
    /// \return
    ///
    static int connect(int sockfd,std::string ip,int port);

    ///
    /// \brief send
    /// \param sockfd
    /// \param buffer
    /// \param len
    /// \return
    ///
    static int send(int sockfd, const char* buffer,int len);

    ///
    /// \brief recv
    /// \param sockfd
    /// \param buffer
    /// \param len
    /// \return
    ///
    static int recv(int sockfd, char* buffer,int len);

    ///
    /// \brief close
    /// \param sockfd
    /// \return
    ///
    static int close(int sockfd);
    ///
    /// \brief check
    /// \param sockfd
    /// \return
    ///
    static int check(int sockfd);


    ///
    /// \brief getpeer
    /// \param sockfd
    /// \param ip
    /// \param port
    /// \return
    ///
    static int getpeer(int sockfd,char* ip,int& port);
    static int getpeer(int sockfd, std::string& ip,int& port)
    {
        char buf[32]={0};
        int ret = getpeer(sockfd,buf,port);
        ip=buf;
        return ret;
    }
    ///
    /// \brief getlocal
    /// \param sockfd
    /// \param ip
    /// \param port
    /// \return
    ///
    static int getlocal(int sockfd, char* ip,int& port);
    static int getlocal(int sockfd, std::string& ip,int& port)
    {
        char buf[32]={0};
        int ret = getlocal(sockfd,buf,port);
        ip=buf;
        return ret;
    }
    ///
    /// \brief getConnectName
    /// \param sockfd
    /// \return
    ///
    static int getSocketName(int sockfd,char* buffer,int size);


    ///
    /// \brief setNoblock
    /// \param sockfd
    /// \param on
    /// \return
    ///
    static int setNoblock(int sockfd, bool on);

    ///
    /// \brief setNoTcpDelay
    /// \param sockfd
    /// \param on
    /// \return
    ///
    static int setNoTcpDelay(int sockfd, bool on);

    ///
    /// \brief setKeepAlive
    /// \param sockfd
    /// \param on
    /// \return
    ///
    static int setKeepAlive(int sockfd, bool on);

    ///
    /// \brief setNoLinger
    /// \param sockfd
    /// \param on
    /// \return
    ///
    static int setNoLinger(int sockfd, bool on);

    ///
    /// \brief setReUseAddr
    /// \param sockfd
    /// \param on
    /// \return
    ///
    static int setReUseAddr(int sockfd, bool on);



    ///
    /// \brief setSendTimeOut
    /// \param sockfd
    /// \param time_ms
    /// \return
    ///
    static int setSendTimeOut(int sockfd, int time_ms);

    ///
    /// \brief setRecvTimeOut
    /// \param sockfd
    /// \param time_ms
    /// \return
    ///
    static int setRecvTimeOut(int sockfd, int time_ms);


    ///
    /// \brief readv
    /// \param fd
    /// \param iov
    /// \param iovcnt
    /// \return
    ///
    static ssize_t readv(int fd, const struct iovec *iov, int iovcnt);


    ///
    /// \brief bind
    /// \param sockfd
    /// \param addr
    /// \param addrlen
    /// \return
    ///
    static int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);


    ///
    /// \brief bind
    /// \param sockfd
    /// \param ip
    /// \param port
    /// \return
    ///
    static int bind(int sockfd,std::string ip, int port);

    ///
    /// \brief listen
    /// \param sockfd
    /// \return
    ///
    static int listen(int sockfd);

    ///
    /// \brief accept
    /// \param sockfd
    /// \param addr
    /// \param addrlen
    /// \return
    ///
    static int accept(int sockfd,struct sockaddr *addr,socklen_t& addrlen);


    ///
    /// \brief fromIpPort
    /// \param ip
    /// \param port
    /// \param addr
    /// \return
    ///
    static int fromIpPort(const char* ip, int port,
                          struct sockaddr_in* addr);

    ///
    /// \brief getSocketError
    /// \param sockfd
    /// \return
    ///
    static int getSocketError(int sockfd);

};
#endif
