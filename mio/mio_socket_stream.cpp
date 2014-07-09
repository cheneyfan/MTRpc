#include "mio_socket_stream.h"
#include "tcpsocket.h"
#include "log/log.h"

namespace mtrpc{

SocketStream::SocketStream():
    _isConnected(false),
    buf_alloc_size(DEFAULT_BUFFER_SIZE)
{
}

SocketStream:: ~SocketStream(){

}

void SocketStream::OnEvent(Epoller* p,uint32_t mask)
{
    int ret = 0;

    TRACE("OnEvent:"<<name<<",mask:"<<EventStatusStr(mask));


    if(mask & EVENT_CLOSE)
    {
        _isConnected = false;
        onClose(p);
        return ;
    }


    if(!_isConnected && (mask &(EVENT_READ|EVENT_WRITE)))
    {
        ret = OnConnect(p);

        if(ret < 0)
        {
            TRACE("OnConnect ret:"<<ret<<", close it");
            onClose(p);
            return;
        }

        _isConnected = true;
    }

    if(mask & EVENT_READ)
    {
        ret |= onReadable(p);
    }

    if(mask & EVENT_WRITE)
    {
        ret |= onWriteable(p);
    }

    if(mask & READ_TIME_OUT)
    {
        ret |= onReadTimeOut(p);
    }

    if(mask & WRITE_TIME_OUT)
    {
        ret |= onWriteimeOut(p);
    }

    if(ret < 0 )
    {
        TRACE("OnEvent mask:"<<mask<<",ret:"<<ret<<", close it");
        onClose(p);
    }
}

int SocketStream::SocketStream::OnConnect(Epoller* p)
{
    TcpSocket::getlocal(_fd, local_ip, local_port);
    TcpSocket::getpeer(_fd, peer_ip, peer_port);
    TRACE("sock:"<<_fd<<",local:"<<local_ip<<":"<<local_port<<",peer:"<<peer_ip<<","<<peer_port);

    if(handerConnected)
        handerConnected->Run(this,p);

    return 0;
}

int SocketStream::onReadable(Epoller *p)
{
    if(handerReadable)
        handerReadable->Run(this,p);

    int read_size = 0;
    int ret  = 0;

    //EDGE triger
    do{
        struct iovec invec[MAX_BUFFER_PIECES];
        int in_num = MAX_BUFFER_PIECES;

        if(!readbuf.GetReciveBuffer(invec,in_num))
        {
            WARN("sock:"<<_fd<<",no buf left in read buf");
            //close
            return -1;
        }


        ret = readv(_fd, invec, in_num);


        if(ret == -1 &&
                ( errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
            //nead read when next event trigger
            TRACE("sock:"<<_fd<<",ret:"<<ret<<",errorno:"<<errno<<",str:"<<strerror(errno));
            break;
        }else if(ret <= 0){

            //some error in socket,need to close
            WARN(name <<",ret:"<<ret<<",errorno:"<<errno<<",str:"<<strerror(errno));
            return -1;
        }

        TRACE(name<<" read:"<<ret);
        readbuf.MoveRecivePtr(ret);

        read_size += ret;
    }while(ret > 0);


    if(read_size > 0)
        OnRecived(p);
    return  ret;
}

int SocketStream::onWriteable(Epoller *p)
{
    if(handerWriteable)
        handerWriteable->Run(this,p);

    if(writebuf.isEmpty())
    {
        TRACE(name<<":no buffer to send, disable write");
        ModEventAsync(p,true,false);
        return 0;
    }

    int ret = 0;

    int write_size = 0;
    do{

        struct iovec outvec[MAX_BUFFER_PIECES];
        int out_num = MAX_BUFFER_PIECES;

        if(!writebuf.GetSendBuffer(outvec,out_num, packetEnd))
        {
            TRACE("packet has send ,sock:"<<_fd
                  <<",writebuf rp:"<<writebuf.readpos.toString()
                  <<",writebuf wp:"<<writebuf.writepos.toString()
                  <<",packetEnd:"<<packetEnd.toString());
            break;
        }

        ret = writev(_fd, outvec, out_num);

        if(ret == -1 &&
                ( errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
            //write at next time
            TRACE(name <<",ret:"<<ret<<",errorno:"<<errno<<",str:"<<strerror(errno));
            break;
        }else if(ret <=0 ) {
            //some error in socket
            WARN(name <<",ret:"<<ret<<",errorno:"<<errno<<",str:"<<strerror(errno));
            return -1;
        }

        TRACE(name<<",send :"<<ret
              <<",writebuf rp:"<<writebuf.readpos.toString()
              <<",writebuf wp:"<<writebuf.writepos.toString()
              <<",packetEnd:"<<packetEnd.toString());

        writebuf.MoveSendPtr(ret);


        if(writebuf.isEmpty())
        {
            TRACE(name<<":no buffer to send, disable write");
            ModEventAsync(p,true,false);
        }

        write_size += ret;

    }while(ret > 0);


    //a packet write ok
    if(write_size > 0)
        OnSended(p);

    return ret;
}


int SocketStream::onClose(Epoller *p)
{
    if(handerClose)
        handerClose->Run(this,p);

    TRACE(name<<" closing");
    DelEventAsync(p);
    return 0;
}


int SocketStream::onReadTimeOut(Epoller* p)
{
    if(handerReadTimeOut)
        handerReadTimeOut->Run(this,p);

    return 0;
}

int SocketStream::onWriteimeOut(Epoller* p)
{
    if(handerWriteimeOut)
        handerWriteimeOut->Run(this,p);
    return 0;
}

int SocketStream::OnRecived(Epoller* p)
{
    return 0;
}

int SocketStream::OnSended(Epoller* p)
{
    return 0;
}

}
