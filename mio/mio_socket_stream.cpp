#include "mio_socket_stream.h"
#include "tcpsocket.h"
#include "log/log.h"

namespace mtrpc{

SocketStream::SocketStream():
    IOEvent(),
    handerConnected(NULL),
    handerReadable(NULL),
    handerWriteable(NULL),
    handerClose(NULL),
    handerReadTimeOut(NULL),
    handerWriteimeOut(NULL),
    handerMessageError(NULL),
    _isConnected(false),
    _close_when_empty(false)
{
}

SocketStream:: ~SocketStream(){

    delete handerConnected;
    delete handerReadable;
    delete handerWriteable;
    delete handerClose;
    delete handerReadTimeOut;
    delete handerWriteimeOut;
    delete handerMessageError;
}

void SocketStream::OnEvent(Epoller* p,uint32_t mask)
{
    int ret = 0;

    TRACE("OnEvent:"<<GetSockName()<<",mask:"<<StatusToStr(mask));


    if(mask & EVENT_CLOSE)
    {
        _isConnected = false;
        //not close directly;

        if(writebuf.isEmpty())
        {
            this->OnClose(p);
        }
        else
        {
            this->_close_when_empty = true;
            this->ModEventAsync(p,true,true);
        }
        return ;
    }

    if(mask & EVENT_ERR)
    {
        _isConnected = false;
        TRACE(GetSockName()<<"error:"<<errno<<","<<strerror(errno))
        this->OnClose(p);
        return ;
    }

    if(!_isConnected && (mask &(EVENT_READ|EVENT_WRITE)))
    {
        ret = this->OnConnect(p);

        if(ret < 0)
        {
            TRACE("OnConnect ret:"<<ret<<", close it");
            this->OnClose(p);
            return;
        }
        _isConnected = true;
    }

    if(mask & EVENT_READ)
    {
        this->OnReadable(p);
    }

    if(mask & EVENT_WRITE )
    {
        this->OnWriteable(p);
    }

    if(mask & READ_TIME_OUT )
    {
        this->OnReadTimeOut(p);
    }

    if(mask & WRITE_TIME_OUT)
    {
        this->OnWriteimeOut(p);
    }

}

int SocketStream::SocketStream::OnConnect(Epoller* p)
{
    TcpSocket::getlocal(_fd, local_ip, local_port);
    TcpSocket::getpeer(_fd, peer_ip, peer_port);

    if(local_ip.size() == 0 ||
            peer_ip.size() ==0)
    {
        TRACE(GetSockName()<<",get error:"<<errno<<","<<strerror(errno));
        return -1;
    }

    if(handerConnected)
        handerConnected->Run(this,p);

    TRACE(GetSockName() <<"connect ok");

    return 0;
}


std::string SocketStream::GetSockName(){
    char name[64]={0};

    if(_isConnected)
         snprintf(name,sizeof(name),"%d(%s:%d->%s:%d)",
             _fd,
             local_ip.c_str(), local_port,
             peer_ip.c_str(), peer_port);
    else
        snprintf(name,sizeof(name),"%d(unconnect)",
            _fd);
    return std::string(name);

}


int SocketStream::OnReadable(Epoller *p)
{
    if(handerReadable)
        handerReadable->Run(this,p);

    int read_size = 0;
    int ret = 0;

    //EDGE triger,read until EAGIN
    do{
        struct iovec invec[MAX_BUFFER_PIECES];
        int in_num = MAX_BUFFER_PIECES;

        if(!readbuf.GetReciveBuffer(invec,in_num))
        {
            WARN(GetSockName()<<",no buf left in read buf");

            this->handerMessageError->Run(this,p,SERVER_READBUFFER_FULL);
            //close
            return -1;
        }

        ret = readv(_fd, invec, in_num);


        if(ret == -1 &&
                ( errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
            //nead read when next event trigger
            TRACE(GetSockName()<<",ret:"<<ret<<",errorno:"<<errno<<",str:"<<strerror(errno));
            break;
        }else if(ret <= 0){

            //some error in socket,need to close
            WARN(GetSockName() <<",ret:"<<ret<<",errorno:"<<errno<<",str:"<<strerror(errno));
            return -1;
        }

        TRACE(GetSockName()<<" read:"<<ret);
        readbuf.MoveRecivePtr(ret);

        read_size += ret;

    }while(ret > 0);


    if(read_size > 0)
        ret = this->OnRecived(p, read_size);

    return  0;
}

int SocketStream::OnWriteable(Epoller *p)
{
    if(handerWriteable)
        handerWriteable->Run(this,p);

    if(writebuf.isEmpty())
    {
        TRACE(GetSockName()<<":no buffer to send, disable write");
        if(_close_when_empty)
            this->OnClose(p);
        else
            ModEventAsync(p,true,false);
        return 0;
    }

    int ret = 0;

    int write_size = 0;

        //EDGE triger,write until EAGIN
    do{

        struct iovec outvec[MAX_BUFFER_PIECES];
        int out_num = MAX_BUFFER_PIECES;

        if(!writebuf.GetSendBuffer(outvec,out_num))
        {
            TRACE(GetSockName()
                  <<",writebuf rp:"<<writebuf.readpos.toString()
                  <<",writebuf wp:"<<writebuf.writepos.toString());
            break;
        }

        ret = writev(_fd, outvec, out_num);

        if(ret == -1
                &&
                 ( errno == EAGAIN
                  || errno == EWOULDBLOCK
                  || errno == EINTR))
        {
            //write at next time
            TRACE(GetSockName() <<",ret:"<<ret<<",errorno:"<<errno<<",str:"<<strerror(errno));
            break;
        }else if(ret <=0 ) {
            //some error in socket
            WARN(GetSockName() <<",ret:"<<ret<<",errorno:"<<errno<<",str:"<<strerror(errno));
            return -1;
        }

        TRACE(GetSockName()<<",send :"<<ret
              <<",writebuf rp:"<<writebuf.readpos.toString()
              <<",writebuf wp:"<<writebuf.writepos.toString());

        writebuf.MoveSendPtr(ret);

        write_size += ret;

    }while(ret > 0);


    //a packet write ok
    if(write_size > 0)
        ret = this->OnSended(p, write_size);

    if(writebuf.isEmpty())
    {
        TRACE(GetSockName()<<":no buffer to send, disable write");
        if(_close_when_empty)
            this->OnClose(p);
        else
            ModEventAsync(p,true,false);
    }

    return ret;
}


int SocketStream::OnClose(Epoller *p)
{
    if(handerClose)
        handerClose->Run(this,p);

    TRACE(GetSockName()<<" closing");
    DelEventAsync(p);
    return 0;
}


int SocketStream::OnReadTimeOut(Epoller* p)
{
    if(handerReadTimeOut)
        handerReadTimeOut->Run(this,p);

    return 0;
}

int SocketStream::OnWriteimeOut(Epoller* p)
{
    if(handerWriteimeOut)
        handerWriteimeOut->Run(this,p);
    return 0;
}

int SocketStream::OnRecived(Epoller* p, uint32_t buffer_size)
{
    TRACE(GetSockName()<<" OnRecived");
    return 0;
}

int SocketStream::OnSended(Epoller* p, uint32_t buffer_size)
{
    TRACE(GetSockName()<<" OnSended");
    return 0;
}

}
