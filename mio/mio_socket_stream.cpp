#include "mio_socket_stream.h"
#include "tcpsocket.h"
#include "log/log.h"

namespace mtrpc{

SocketStream::SocketStream():buf_alloc_size(DEFAULT_BUFFER_SIZE),
    readbuf(DEFAULT_BUFFER_SIZE),
    writebuf(DEFAULT_BUFFER_SIZE)
{
    _fd = TcpSocket::socket();
    TcpSocket::setNoTcpDelay(_fd);
}

int SocketStream::onReadable(Epoller *p)
{

    if(readbuf.GetReciveBufferLeft() < DEFAULT_BUFFER_SIZE  && !readbuf.Extend(buf_alloc_radio))
    {
        WARN("can't Extend buf");
        return -1;
    }

    struct iovec invec[MAX_BUFFER_PIECES];
    int in_num = MAX_BUFFER_PIECES;
    if(!readbuf.GetReciveBuffer(invec,in_num))
    {
        WARN("no buf left in read buf");
        return -1;
    }

    int ret = readv(_fd, invec, in_num);


    if(ret == -1 &&
            ( errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
        //nead read next time
        TRACE(name <<",ret:"<<ret<<",errorno:"<<errno<<",str:"<<strerror(errno));
        return 0;
    }else if(ret <= 0){
        //some error in socket
        WARN(ev.name <<",ret:"<<ret<<",errorno:"<<errno<<",str:"<<strerror(errno));
        return -1;
    }

    TRACE(name<<" read:"<<ret);
    readbuf.MoveRecivePtr(ret);

    return OnRecived(p);
}

int SocketStream::onWriteable(Epoller *p)
{

    if(!writebuf.GetSendBufferUsed())
    {
        TRACE(name<<":no buffer to send, disable write");
        ModEventAsync(p,true,false);
        return 0;
    }

    struct iovec outvec[MAX_BUFFER_PIECES];
    int out_num = MAX_BUFFER_PIECES;

    writebuf.GetReciveBuffer(outvec,out_num);

    int ret = writev(_fd,outvec, out_num);

    if(ret == -1 &&
            ( errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
        //write at next time
        TRACE(name <<",ret:"<<ret<<",errorno:"<<errno<<",str:"<<strerror(errno));
        return 0;
    }else if(ret <=0 ) {
        //some error in socket
        WARN(name <<",ret:"<<ret<<",errorno:"<<errno<<",str:"<<strerror(errno));
        return -1;
    }

    TRACE(name<<" write:"<<ret);

    writebuf.MoveSendPtr(ret);


    if(!writebuf.GetSendBufferUsed())
    {
        TRACE(name<<":no buffer to send, disable write");
        ModEventAsync(p,true,false);
        return 0;
    }

    return OnSended(p);
}


int SocketStream::onClose(Epoller *p){

     DelEventAsync(p);
}

}
