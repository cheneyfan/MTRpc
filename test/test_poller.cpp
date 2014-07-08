#include "stdio.h"
#include "log/log.h"
#include "thread/workpool.h"
#include "common/signalhelper.h"
#include "mio/mio_timer.h"
#include "common/timerhelper.h"

#include "mio/mio_poller.h"
#include "thread/workpool.h"

using namespace mtrpc;

class C :public IOEvent {
public:

    virtual void OnEvent(Epoller* p, uint32_t event_mask) {

        TRACE("event_mask:"<<EventStatusStr(event_mask));
        std::cout<<time(NULL)<<",event_mask:"<<EventStatusStr(event_mask)<<std::endl;
        //SetReadTimeOutAsync(p,1+time(NULL));
    }

};

int main(int argc,char*argv[]){

    Json::Value conf;
    LogBacker::Init(conf);

    WorkGroup* group =new WorkGroup();

    Epoller *poller = new Epoller();



    group->Init(5);

    group->Post(NewExtClosure(poller,&Epoller::Poll));

    C* c =new C();

    c->group = group;
    //c->AddEventASync(poller,true,false);

    c->SetReadTimeOutAsync(poller,1);


    group->join();
    return 0;

}
