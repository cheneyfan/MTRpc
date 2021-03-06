#include <unistd.h>

#include "log.h"

#include "thread/workpool.h"
#include "common/singleton.h"
#include "common/signalhelper.h"

#define ENABLE_LOG
#ifdef ENABLE_LOG
const char* DEBUG_LEVEL = "DEBUG";
const char* TRACE_LEVEL = "TRACE";
const char* INFO__LEVEL = "INFO_";
const char* WARN__LEVEL = "WARN_";
const char* ERROR_LEVEL = "ERROR";
#else
const char* DEBUG_LEVEL = NULL;
//const char* TRACE_LEVEL = NULL;
const char* INFO__LEVEL = NULL;
const char* WARN__LEVEL = NULL;
const char* ERROR_LEVEL = NULL;
#endif


namespace mtrpc {


//char * LogLevel::TRACE_LEVEL = "TRACE";

const char* SPLIT = " - ";
const char* ENDL  = "\n";

__thread uint32_t LogHelper::tid = 0;
__thread char LogHelper::tidstr[8]={0};

__thread time_t LogHelper::time = 0;
__thread time_t LogHelper::last_create = 0;
__thread char LogHelper::datebuf[24]={0};
__thread char LogHelper::format_datebuf[24]={0};

bool is_sync = true;



void LogEntry::Reset(const char* level,
           const char * file,
           int line,
           const char *func)
{
    lev = level;
    fil = basename(file);
    fun = func;

#ifndef TRACE_STACK

    snprintf(lin,sizeof(lin),":%d:",line);

#else
    char *n = lin;
    snprintf(n, sizeof(lin),":%d:",line);
    n += sizeof(":%d:") -1;
    SignalHelper::getBacktrace(n,sizeof(lin),2,3);
#endif
    buf.Reset();
}



void LogEntry::toIovec(iovec* v,uint32_t& size)
{

    v[0].iov_base = (void*)(lev);
    v[0].iov_len  = 5;

    v[1].iov_base = prefix;
    v[1].iov_len  = strlen(prefix);


    v[2].iov_base = buf.buf;
    v[2].iov_len  = strlen(buf.buf);

    v[3].iov_base = (void*)(SPLIT);
    v[3].iov_len  = 3;

    v[4].iov_base = (void*)(fil);
    v[4].iov_len  = strlen(fil);


    v[5].iov_base = (void*)(lin);
    v[5].iov_len  = strlen(lin);


#ifndef TRACE_STACK
    v[6].iov_base = (void*)(fun);
    v[6].iov_len  = strlen(fun);


    v[7].iov_base = (void*)(ENDL);
    v[7].iov_len  = 1;

    size = 8;
#else

    v[6].iov_base = (void*)(ENDL);
    v[6].iov_len  = 1;

    size = 7;
#endif


}



void LogHelper:: OutPut(LogEntry* e)
{

    CacheManger& cm = ThreadLocalSingleton<CacheManger>::instance();
    // first log need to register to the backer
    if(!tid)
    {
        tid = gettid();
        snprintf(tidstr,sizeof(tidstr),"%d",tid);
  //      LogBacker::registerThread(&cm);
    }

    //TODO opt the function
    struct timeval tp;
    gettimeofday(&tp,NULL);

    if(tp.tv_sec != time)
    {
        time = tp.tv_sec;
        struct tm tmp;
        localtime_r(&tp.tv_sec, &tmp);
        strftime(datebuf, sizeof(datebuf),"%F %T",&tmp);
        strftime(format_datebuf,sizeof(format_datebuf), "%Y-%m-%d-%H",&tmp);
    }

    snprintf(e->prefix,sizeof(e->prefix)," - %s.%03ld - %s - ",datebuf,tp.tv_usec/1000, tidstr);

    if(is_sync){
        WriteLock<MutexLock> __(LogBacker::spin);
        LogBacker::OutPut(e);
        delete e;
    }else
        cm._loglist.push(e);
}



CacheManger::CacheManger()
    :next(NULL)
{
}

CacheManger::~CacheManger(){

//    LogBacker::unregisterThread(this);
}

LogEntry* CacheManger::mallocEntry(){

    return new LogEntry();
}

void CacheManger::free(LogEntry *e){
    delete e;
}

LogBuffer* CacheManger::mallocBuffer(uint32_t size){

    return new LogBuffer(size);
}

void CacheManger::free(LogBuffer* b){
    delete b;
}


int LogBacker::fd = 1;
char LogBacker::logfile[256]={0};
Worker*  LogBacker::worker = new Worker();
SpinList<LogEntry*,SpinLock> LogBacker::_loglist;
CacheManger * LogBacker::next = NULL;
MutexLock LogBacker::spin;
bool LogBacker::isruning = true;
std::string LogBacker::dirname = "log";
bool LogBacker::isinit = false;

int LogBacker::Init(Json::Value & conf __attribute__((unused))){

    memcpy(logfile,"a.log",sizeof("a.log"));

    is_sync = false;

    fd = open(logfile,
              O_CREAT|O_TRUNC|O_WRONLY,
              S_IRWXU|S_IRWXG|S_IRWXO);


    MioTask* task = NewExtClosure(LogBacker::DumperLogger, NULL);
    worker->RunTask(task);
    worker->start();

    mtrpc::SignalHelper::registerCallback(LogBacker::Stop);

    // init logger in main thread
    INFO("start logger file:"<<logfile);

    atexit(LogBacker::Close);

    return 0;
}

int LogBacker::Init()
{
    if (0 != access(dirname.c_str(), F_OK))
    {
       if (0 !=  mkdir(dirname.c_str(), S_IRWXU|S_IRWXG|S_IRWXO))
       {
           std::cout<<"mkdir dir log failed, errno is "<<errno<<std::endl;
           return -1;
       }
    }

    isinit = true;
    return 0;
}


void LogBacker::Stop(void*){

    isruning = false;
    worker->join();
}

void LogBacker::Close(){

    isruning = false;
    worker->join();

    CacheManger& cm = ThreadLocalSingleton<CacheManger>::instance();

    if(!cm._loglist.empty()){
        _loglist.Merge(cm._loglist);
    }

    DumperDefault(NULL, 0);
    close(fd);
    delete worker;
}


void LogBacker::Join(){
    worker->join();
}

void LogBacker::registerThread(CacheManger* cm){
    WriteLock<MutexLock> sp(spin);
    cm->next = next;
    next = cm;
}

void LogBacker::unregisterThread(CacheManger* cm){

    WriteLock<MutexLock> sp(spin);

    //move the logger into main thread
    if(!cm->_loglist.empty())
    {
        if(next)
            next->_loglist.Merge(cm->_loglist);
        else
            _loglist.Merge(cm->_loglist);
    }

    if(next == cm)
    {
        next = NULL;
        return ;
    }

    //remove tmp
    CacheManger* tmp = next;
    for(; tmp && tmp->next != cm; tmp = tmp->next)
    {
        if(tmp->next == cm)
        {
            tmp->next = tmp->next->next;
            break;
        }
   }
}



void LogBacker::OutPut(LogEntry* e){

    struct iovec iov[10];
    uint32_t iov_size = 0;

    // create new file every 3600 second
    if(LogHelper::time - LogHelper::last_create > 3600)
    {
        if(fd > 1)
            close(fd);

        LogHelper::last_create = LogHelper::time;

        snprintf(logfile,
                 sizeof(logfile),
                 "info.log.%s",
                 LogHelper::format_datebuf);

        fd = open(logfile,
                  O_CREAT|O_WRONLY|O_APPEND,
                  S_IRWXU|S_IRWXG|S_IRWXO);
    }



    e->toIovec(iov,iov_size);

    ::writev(fd, iov, iov_size);
}

void LogBacker::DumperLogger(void *ctx __attribute__((unused)))
{

    //Trace every thread
    while(isruning)
    {

        CacheManger * m = next;
        SpinList<LogEntry*,SpinLock> tmp;

        bool isout = false;

        {
            WriteLock<MutexLock> sp(spin);
            while(m)
            {
                if(!m->_loglist.empty())
                {
                    tmp.Merge(m->_loglist);;
                    isout =true;
                }

                //move next
                {
                    m = m->next;
                }
            }
        }

        LogEntry* e  = NULL;
        while(tmp.pop(e) && e)
        {
            LogBacker::OutPut(e);
            delete e;
        }

        if(isout)
            fdatasync(fd);
        else
            usleep(1000*10);

    }
}


int LogBacker::DumperDefault(void *ctx __attribute__((unused)) , int error __attribute__((unused)))
{

    LogEntry* e  = NULL;

    while(LogBacker::_loglist.pop(e) && e)
    {
        LogBacker::OutPut(e);
        delete e;
    }

    fdatasync(fd);
    return 0;
}

}

