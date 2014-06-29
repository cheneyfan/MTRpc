#include "stdio.h"
#include "iostream"
#include <sys/time.h>


#include "log/log.h"
#include "common/timerhelper.h"

#include "common/signalhelper.h"
#include "thread/workpool.h"

using namespace mtrpc;
int pp =0;

#define TF "%s %d %u %ld %lu %f %lf %c %p %% "
#define TD "a",10,-1,10000000,-10,1.0,2.0,'c',&pp


#define S1 "%s %d %u %ld %lu %f %lf %c %p %% "<<"a"<<10<<-1<<10000000<<-10<<1.0<<2.0<<'c'<<&pp
#define TIMES 100000

int test_snprintf(){

    TimeMoniter t1(0,__FUNCTION__);

    for(int i =0;i<TIMES;++i)
    {
        char buff[100];
        sprintf(buff,TF,TD);
    }

    char buff[100];
    sprintf(buff,TF,TD);
    std::cout<<__FUNCTION__<<":"<<buff<<std::endl;
    return 0;
}

int test_std_cout(){

    TimeMoniter t1(0,__FUNCTION__);
    for(int i =0;i<TIMES;++i)
    {
        std::stringstream str;
        str<<S1;
    }
    std::stringstream str;
    str<<S1;
    std::cout<<__FUNCTION__<<":"<<str.str()<<std::endl;
    return 0;
}

int test_log_Fomart(){
    TimeMoniter t1(0,__FUNCTION__);
    for(int i =0;i<TIMES;++i)
    {
        LogEntry e;
        e.Format(TF,TD);
    }

    DEBUG_FMG(TF,TD);
    return 0;
}

int test_log_Stream(){
    TimeMoniter t1(0,__FUNCTION__);


    for(int i =0;i<TIMES;++i)
    {
        LogEntry e;
        e<<S1;
    }
    DEBUG(S1);
    return 0;
}

int main(int argc,char* argv[]){

    Json::Value conf;
    LogBacker::Init(conf);



    char buff[100];
    sprintf(buff,"1223%% %p ",&pp);
    std::cout<<__FUNCTION__<<":"<<buff<<std::endl;


    //SignalHelper::install_sig_action();

    test_snprintf();
    test_std_cout();
    test_log_Fomart();
    test_log_Stream();


    return 0;
}
