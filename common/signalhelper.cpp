#include "signalhelper.h"

#include "log/log.h"
#include <stdio.h>
#include <stdlib.h>

#include <signal.h>
#include <execinfo.h>
#include <errno.h>
#include <iostream>
#include <cxxabi.h>

using namespace mtrpc;
/// static method
std::vector<Closure> SignalHelper::callbacks;

// sigaction needs this fuction
void SignalHelper::signal_handler(int signal)
{
    INFO_FMG("pid:%d,catch sig:%d,begin stop",getpid(),signal);
    std::cerr<<"pid:"<<getpid()<<",catch sig: "<<signal<<",begin stop"<<std::endl;
    for(unsigned int i =0; i<callbacks.size();++i)
    {
        callbacks[i]();
    }
}

void SignalHelper::core_handler(int signal){

    //INFO("pid:"<<getpid()<<",catch sig: "<<signal<<",begin core dump");

    std::cerr<<"pid:"<<getpid()<<",catch sig: "<<signal<<",begin core dump"<<std::endl;

    char* stack[32] = {0};


    int depth = backtrace(reinterpret_cast<void**>(stack),sizeof(stack)/sizeof(stack[0]));

    if(depth)
    {
        char ** symbols = backtrace_symbols(reinterpret_cast<void**>(stack),depth);

        std::cerr<<"back_trace:"<<std::endl;
        for(int i=1; symbols !=NULL && i<depth;i++)
        {
            char exename[256]={0};
            char addr [32]={0};

            sscanf(symbols[i],"%256[^(]%*[^[][%32[^]]", exename, addr);
            char cmd[256]= {0};

            sprintf(cmd,"addr2line %s -e %s", addr, exename);

            std::cerr<<"["<<i<<"]:"<<symbols[i]<<" from:";
            system(cmd);
        }

        free(symbols);
    }

    raise(signal);
}

void SignalHelper::getBacktrace(char* buffer, int size,int start,int num){

    char* stack[10] = {0};
    int depth = backtrace(reinterpret_cast<void**>(stack),sizeof(stack)/sizeof(stack[0]));

    num+=start;
    depth = depth < num ? depth : num;
    char * end = buffer+size;

    char ** symbols = backtrace_symbols(reinterpret_cast<void**>(stack),depth);
    for(int i=start; symbols !=NULL && i<depth;i++)
    {

        char exename[256]={0};
        char func[256]={0};
        //printf("%s",symbols[i]);
        sscanf(symbols[i],"%256[^(](%256[^+)]", exename, func);

        int flongth = strlen(func);
        if( flongth == 0)
            continue;

        if(flongth< 2)
        {
            strncpy(buffer, func, flongth);
            buffer += flongth;
            continue;
        }

        if(func[0] != '_' && func[1]!='Z' && (buffer +flongth < end))
        {
            strncpy(buffer, func, flongth);
            buffer+= flongth;
            continue;
        }

        size_t longth = 256;
        int status = 0;
        abi::__cxa_demangle(func, exename, &longth, &status);
        //std::cout<<"sy:"<<symbols[i]<<",fun:"<<func<<",d:"<<exename<<",l:"<<longth<<",status:"<<status<<std::endl;
        char funcname[256]={0};
        char paraname[256]={0};
        sscanf(exename,"%256[^(](%256[^+)]", funcname, paraname);

        int pos = 0;
        int last = 0;
        while(funcname[pos++]!='\0'){if(funcname[pos] == ':') last=pos;}
        if(last)
            memmove(funcname,funcname+last+1, pos-last-1);

        longth =strlen(funcname);

        if(status==0  && (buffer +longth < end))
        {
            strncpy(buffer,"<",1);
            buffer+= 1;
            strncpy(buffer,funcname,longth);
            buffer+= longth;

        }
    }

    *buffer='\0';

    free(symbols);

}

///
/// \brief install_sig_action
/// \return
///
int SignalHelper::install_sig_action()
{
    /// user to kill
    signal(SIGQUIT, SignalHelper::signal_handler);
    signal(SIGINT,  SignalHelper::signal_handler);
    signal(SIGTERM, SignalHelper::signal_handler);
    signal(SIGUSR1, SignalHelper::signal_handler);


    // ignore SIGPIPE
    signal(SIGPIPE, SIG_IGN);

    struct sigaction act;
    memset(&act,0,sizeof(act));
    act.sa_handler = & SignalHelper::core_handler;
    act.sa_flags |= SA_RESETHAND;

    sigaction(SIGABRT, &act, NULL);
    sigaction(SIGSEGV, &act, NULL);
    sigaction(SIGBUS,  &act, NULL);
    sigaction(SIGFPE,  &act, NULL);


    enable_core_dump(true, -1);
    return 0;
}

int SignalHelper::enable_core_dump(bool enabled, int core_file_size)
{
    if (enabled)
    {
        struct rlimit rlim;
        rlim.rlim_cur = (core_file_size <= 0)? RLIM_INFINITY: core_file_size;
        rlim.rlim_max = rlim.rlim_cur;

        if (-1 == setrlimit(RLIMIT_CORE, &rlim))
        {
            WARN("only root can set limit core file num,errno:"<<errno<<","<<strerror(errno));
        }

        getrlimit(RLIMIT_CORE,&rlim);
        INFO("core limit,cur:"<<rlim.rlim_cur<<",max:"<<rlim.rlim_max);

        // set limit only root use
        rlim.rlim_cur = 655350;
        rlim.rlim_max = 655350;
        if (-1 == setrlimit(RLIMIT_NOFILE,&rlim))
        {
            WARN("only root can set limit open file num,errno:"<<errno<<","<<strerror(errno));
        }

        getrlimit(RLIMIT_NOFILE,&rlim);
        INFO("open file limit,cur:"<<rlim.rlim_cur<<",max:"<<rlim.rlim_max);
    }

    if (-1 == prctl(PR_SET_DUMPABLE, enabled? 1: 0))
        return -1;

    return 0;
}
