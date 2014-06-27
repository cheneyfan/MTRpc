#ifndef _MIO_CONNECTION_H_
#define _MIO_CONNECTION_H_


#include<string>
#include <map>

#include "log.h"
#include "mio_task.h"
#include "mio_event.h"
#include "mio_buffer.h"
#include "mio_poller.h"

namespace mio2 {



class WorkGroup;

template<class Handler>
class Connection {
public:

    Connection(int sock){
        ev._fd = sock;
        ev.ev.data.ptr = &ev;
        ev.onEvent = Closure::From< Connection<Handler>,Epoller*,uint32_t,&Connection<Handler>::onEvent>(this);
        isConnected = false;
    }

    void onEvent(Epoller* p,uint32_t events)
    {
        int premask = __sync_fetch_and_and(&ev.events, events);

        // event is in work pool queue or begin to run
        if(premask & EVENT_PENDING )
        {
            return ;
        }
        if(premask & EVENT_PROCESSING )
        {
            return ;
        }

        //push to workpool
        __sync_fetch_and_and(&ev.events,EVENT_PENDING);
        poller = p;
        Closure c = Closure::From<Connection<Handler>, &Connection<Handler>::processEvent>(this);
        group->Post(c);
    }


    void processEvent()
    {

        do{
            int mask = __sync_lock_test_and_set(&ev.events, EVENT_PROCESSING);

            int ret =0;
            if(!isConnected)
            {
                ret = handler.OnConnect(ev._fd,output);
                if(!output.Empty())
                    poller->ModEvent(&ev,true,true);
            }

            if(!ret)
                ret = dispatch(poller,mask);

            if(ret)
                onClose(poller);

            //until no event pending in processing
        }while(__sync_val_compare_and_swap (&ev.events, EVENT_PROCESSING,0 ) );

    }

    int  dispatch(Epoller* p,uint32_t mask)
    {

        int ret = 0;
        if(mask & EVENT_READ)
        {
            ret |= onRead(p);
        }

        if(mask& EVENT_WRITE)
        {
            ret |= onWrite(p);
        }

        if(mask &EVENT_CLOSE)
        {
            ret |= -1;
        }

        if(mask&READ_TIME_OUT)
        {
            ret |= onReadTimeout(p);
        }

        if(mask&WRITE_TIME_OUT)
        {
            ret |= onWriteTimeout(p);
        }

        return ret;
    }


    int onRead(Epoller* p)
    {

        char tmp[4096];
        struct iovec invec[2];

        invec[0].iov_base = input.wptr;
        invec[0].iov_len  = input.TailSize();

        invec[0].iov_base = tmp;
        invec[0].iov_len  = 4096;

        int ret = readv(ev._fd, invec, 2);

        if(ret == -1 && ( errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
            //nead read next time
            TRACE(ev.name <<",ret:"<<ret<<",errorno:"<<errno<<",str:"<<strerror(errno));
            return 0;
        }else if(ret <= 0){
            WARN(ev.name <<",ret:"<<ret<<",errorno:"<<errno<<",str:"<<strerror(errno));
            return -1;
        }

        TRACE(ev.name<<" read:"<<ret);

        // read small buffer
        if(ret <=invec[0].iov_len)
        {
            input.WriteSkip(invec[0].iov_len);

            // read large buffer
        }else{
            input.WriteSkip(invec[0].iov_len);
            input.Append(tmp, ret - invec[0].iov_len);
        }

        // process head
        int headlen = handler.GetHeadLen(input);
        if(headlen == -1)
            return 0;

        // process body
        int bodylen = handler.GetBodyLen(input);
        if(bodylen == -1)
            return 0;

        if(input.MidSize() <  bodylen)
            return 0;

        handler.OnPacket(input,output);
        if(!output.Empty())
            poller->ModEvent(&ev,true,true);

        return 0;
    }

    int  onWrite(Epoller* p){

        // assert
        if(output.Empty())
        {
            poller->ModEvent(&ev,true,false);
            WARN(ev.name<<" write a empty buffer");
            return 0;
        }


        std::vector<iovec> outvec;
        output.GetIovec(outvec);

        int ret = writev(ev._fd,outvec.data(),outvec.size());

        if(ret == -1 && ( errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) {
            //nead read next time
            TRACE(ev.name <<",ret:"<<ret<<",errorno:"<<errno<<",str:"<<strerror(errno));
            return 0;
        }else if(ret <=0 ) {
            WARN(ev.name <<",ret:"<<ret<<",errorno:"<<errno<<",str:"<<strerror(errno));
            return -1;
        }

        TRACE(ev.name<<" write:"<<ret);

        output.Consume(ret);
        if(output.Empty())
        {
            poller->ModEvent(&ev,true,false);
        }
        return 0;
    }

    void onClose(Epoller* p){
        TRACE(ev.name);

        if(isConnected)
        {
            handler.onClose(ev._fd);
            isConnected = false;

        }

        // must run in poll thread
        Closure c = Closure::From<Connection<Handler>, &Connection<Handler>::release>(this);
        p->runTask(c);
    }

    int onReadTimeout(Epoller* p){
        TRACE(ev.name);
        return 0;
    }

    int  onWriteTimeout(Epoller* p){
        TRACE(ev.name);
        return 0;
    }

    /// run in epoll
    /// \brief release
    ///
    void release(){
        //delete poll
        poller->DelEvent(&ev);
        delete this;
    }

public:
    IOEvent ev;
    WorkGroup* group;

    bool isConnected;

    Handler  handler;

    MioBufferIn input;
    MioBufferOut output;
    Epoller* poller;
};


}
#endif
