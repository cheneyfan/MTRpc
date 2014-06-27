#include "stdio.h"
#include "iostream"
#include <sys/time.h>


#include "mio2/log.h"
#include "mio2/timerhelper.h"
#include "mio2/workpool.h"

using namespace mio2;



uint64_t i =0;


void hello1(void* ctx){
    ++i;
}

class B {
public:

    B():x(11){
    }

    void hello2(){
        ++i;
    }

private:
    int x ;
};


int t1(){
    TimeMoniter tm(0,__FUNCTION__);
    int a=0;

    for(int i=0;i<10000000;++i)
        hello1(&a);
    return 0;
}

int t2(){
    TimeMoniter tm(0,__FUNCTION__);
    int a=0;
    Closure t =Closure::From(hello1,((void*)&a));
    for(int i=0;i<10000000;++i)
        t();
    return 0;
}


int t3(){

    TimeMoniter tm(0,__FUNCTION__);
    B b;

    for(int i=0;i<10000000;++i)
        b.hello2();
    return 0;
}


int t4(){
    TimeMoniter tm(0,__FUNCTION__);
    B b;
    Closure t = Closure::From<B,&B::hello2>(&b);
    for(int i=0;i<10000000;++i)
        t();
    return 0;
}

template<B& a>
int hello(){
    a.hello2();
    return 0;
}



int main(int argc,char* argv[]){

    Json::Value conf;
    LogBacker::Init(conf);

    t1();
    t2();
    t3();
    t4();
    //std::cout<<i<<std::endl;
    B b;
    //hello<b>();
    return i;
}
