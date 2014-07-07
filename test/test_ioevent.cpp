#include "stdio.h"
#include <iostream>

#include "mio/mio_event.h"
#include "mio/mio_poller.h"
#include "thread/workpool.h"

using namespace mtrpc;

class C :public IOEvent{
public:
    virtual void OnEvent(Epoller* p, uint32_t event_mask){

        std::cout<<"say hehe:"<<name<<std::endl;
    }
};

int main(int argc,char* argv[]){


    IOEvent * ev = new C();
    ev->SetEvent(true,true);

    ev->OnEventAsync(NULL,0);



    return 0;
}
