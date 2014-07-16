#include <stdio.h>
#include "log/log.h"
#include "thread/workpool.h"
#include "common/signalhelper.h"

class A{

public:
    A(const char * n):
    name(n)
    {
         printf("born\n");

        DEBUG("born:"<<name);
    }

    ~A(){

        printf("dead\n");

        DEBUG("dead:"<<name);
    }

   std::string name;
};


void hehe(void){

    printf("exit\n");

}
__thread A* at = NULL;
A am("global");

int main(int argc,char*argv[]){

    atexit(hehe);
    mtrpc::Json::Value conf;
    mtrpc::LogBacker::Init(conf);

    A am("main");

    //mio2::WorkGroup g;
    //g.Init(3);
    //mio2::SignalHelper::registerCallback<mio2::WorkGroup,&mio2::WorkGroup::Stop>(&g);


    //g.join();
}
