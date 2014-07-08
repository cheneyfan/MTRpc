#include "stdio.h"
#include <iostream>

#include "mio/mio_event.h"
#include "mio/mio_poller.h"
#include "thread/workpool.h"
#include "log/log.h"
#include "common/atomic.h"

using namespace mtrpc;


void TriggerTask(Epoller* p,IOEvent * ev,int i){

   int e[] = {EVENT_READ,EVENT_WRITE,EVENT_CLOSE,READ_TIME_OUT,WRITE_TIME_OUT};

  // int i = rand() % 5;
   i=i%5;

   ev->OnEventAsync(p,e[i]);
   ev->OnEventAsync(p,e[(i+1)%5]);
   ev->OnEventAsync(p,e[(rand())%5]);
}

class C :public IOEvent{
public:
    virtual void OnEvent(Epoller* p, uint32_t event_mask){

        b.increment();

        std::cout<<Worker::CurrentWorker()->tid<<",say hehe:"<<name<<",evmask:"<<EventStatusStr(event_mask)<<std::endl;
        //usleep(100000);
   int e[] = {EVENT_READ,EVENT_WRITE,EVENT_CLOSE,READ_TIME_OUT,WRITE_TIME_OUT};
        int i =0;
        for(int i=0;i<5;++i)
        {
            if( e[i] & event_mask)
            {
                ExtClosure<void(void)>* t = NewExtClosure(TriggerTask,p,this,i+1);
                p->PostTask(t);
                break;
            }
        }

        usleep(100);
        assert(b.get() == 1);
        b.decrement();

        usleep(100);
        assert(b.get() == 0);


    }

    AtomicInt32 b;
};



int main(int argc,char* argv[]){


    srand(time(NULL));

    Json::Value conf;
    LogBacker::Init(conf);


    IOEvent * ev = new C();
    ev->SetEvent(true,true);

    WorkGroup* group =new WorkGroup();

    Epoller *poller = new Epoller();

    ev->group = group;

    group->Init(5);

    group->Post(NewExtClosure(poller,&Epoller::Poll));

    sleep(1);
    for(int i=0;i<1;i++)
       ev->OnEventAsync(poller,EVENT_READ);

    group->join();

    return 0;
}
