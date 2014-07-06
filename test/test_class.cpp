#include "stdio.h"
#include <iostream>

class A{


public:

    void mock(){
        this->say();
    }
    virtual void say() =0;
};

class B :public A{

public:
    virtual void  say(){

        std::cout<<"say hehe"<<std::endl;
    }
    int aa;
};


#include "mio/mio_event.h"

using namespace mtrpc;

class C :public IOEvent{
public:


    virtual void OnEvent(Epoller* p, uint32_t event_mask){

        std::cout<<"say hehe"<<std::endl;
    }
};
int main(int argc,char* argv[]){

    A * a = new B();
    a->mock();

    IOEvent * ev = new C();

    ev->OnEventAsync(NULL,0);
    return 0;
}
