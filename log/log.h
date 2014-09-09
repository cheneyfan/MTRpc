#ifndef __MTRPC_LOG_H_
#define __MTRPC_LOG_H_


#include <stdio.h>
#include <sys/uio.h>
#include <stdint.h>

#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdarg.h>
#include <algorithm>

#include "json/configure.h"

#define LOGBUFFER_MAX_NUM 10000
#define LOGBUFFER_DEFAULT_SIZE 4096
#define LOGENTRY_MAX_NUM 1000

#define gettid() syscall(SYS_gettid)
#define forceinline __inline__ __attribute__((always_inline))



#include "common/spinlist.h"

#include "common/covert.h"

//#define TRACE_STACK
#define _SYNC_LOG_

#define __PRETTY_FUNCTION__ __FUNCTION__
#ifndef TRACE_STACK
#endif


extern const char* DEBUG_LEVEL;
extern const char* TRACE_LEVEL;
extern const char* INFO__LEVEL;
extern const char* WARN__LEVEL;
extern const char* ERROR_LEVEL;

namespace mtrpc {


extern const char* SPLIT;
extern const char* ENDL;


class LogBuffer
{
public:
    LogBuffer():
        buf(NULL),
        size(0),
        next(NULL)
    {
    }

    LogBuffer(uint32_t s):
        buf(new char[s]),
        ptr(buf),
        size(s),
        next(NULL){
    }

    ~LogBuffer(){

        Reset();
        delete[] buf;
    }
    forceinline uint32_t left(){
        return size - (ptr - buf);
    }

    forceinline void Reset()
    {
        ptr = buf;
        while(next)
        {
            LogBuffer* n = next->next;
            delete next;
            next = n ;
        }
    }

public:
    char * buf;
    char * ptr;
    uint32_t size;
    LogBuffer* next;
};


class LogEntry{

public:
    const char* lev;
    const char* fil;
    const char* fun;
    LogBuffer buf;
    char prefix[64];

#ifndef TRACE_STACK
    char lin[16];
#else
    char lin[512];
#endif

public:

    LogEntry():
        buf(LOGBUFFER_DEFAULT_SIZE)
    {
    }

    ///
    /// \brief Reset
    /// \param level
    /// \param file
    /// \param line
    /// \param func
    ///
    void Reset(const char* level,
               const char * file,
               int line,
               const char *func);


    ///
    /// \brief toIovec
    /// \param v
    /// \param size
    ///
    void toIovec(iovec* v,uint32_t& size);
    
    template<typename T>
    void Format(const T* t){
        if(buf.size < 32 )
            return;
        *buf.ptr ++ ='0';
        *buf.ptr ++ ='x';
        int len = convertHex(buf.ptr,t) +2;
        buf.ptr  += len ;
        buf.size -= len;

    }
#ifdef __i386__
    template<char>
#endif
    void Format(char* str)
    {
        int len = strlen(str);
        if(buf.size < len)
            return;

        memcpy(buf.ptr,str,len);
        buf.ptr  += len;
        buf.size -= len;
    }

#ifdef __i386__
    template<const char>
#endif
    void Format(const char* str)
    {
        int len = strlen(str);
        if(buf.size < len)
            return;

        memcpy(buf.ptr,str,len);
        buf.ptr  += len;
        buf.size -= len;
    }
    /*
    void Format(char* str)
    {
        Format((const char*)str);
    }
*/
    void Format(char c)
    {
        if(buf.size < 1)
            return;

        *buf.ptr++ = c;
        buf.size -= 1;
    }


    void Format(int d){
        if(buf.size < 16 )
            return;

        int len = convert(buf.ptr,d);
        buf.ptr  += len ;
        buf.size -= len;
    }

    void Format(unsigned int  d){
        if(buf.size < 16 )
            return;

        int len = convert(buf.ptr,d);
        buf.ptr  += len ;
        buf.size -= len;
    }
    
    //#ifdef __i386__
    void Format(long unsigned int  d){
        if(buf.size < 16 )
            return;

        int len = convert(buf.ptr,d);
        buf.ptr  += len ;
        buf.size -= len;
    }

    //#endif

    void Format(long long d){
        if(buf.size < 32 )
            return;

        int len = convert(buf.ptr,d);
        buf.ptr  += len ;
        buf.size -= len;
    }

    void Format(unsigned long long  d){

        if(buf.size < 32 )
            return;

        int len = convert(buf.ptr,d);
        buf.ptr  += len ;
        buf.size -= len;

    }

    void Format(float d){
        if(buf.size < 32 )
            return;
        int len = snprintf(buf.ptr,buf.size,"%f",d);
        buf.ptr  += len ;
        buf.size -= len;
    }

    void Format(double d){
        if(buf.size < 32 )
            return;
        int len = snprintf(buf.ptr,buf.size,"%lf",d);
        buf.ptr  += len ;
        buf.size -= len;
    }

    
#ifdef __x86_64__
    template<typename T, typename... Args>
    void Format(const char *str, T value, Args... args)
    {
        char * s = const_cast<char*>(str);

        while (*s) {

            if(*s != '%')
            {
                *buf.ptr++ = *s++;
                continue;
            }

            s++;
            char format[1024]={'%'};
            char *ptr = format + 1;

            while(*s)
            {
                switch(*s){
                case 'd':case 'i':
                case 'o':case 'u':case 'x':case 'X':
                case 'e':case 'E':
                case 'f':case 'F':
                case 'g':case 'G':
                case 'a':case 'A':
                case 'p':

                    *ptr++= *s++;
                    *ptr ='\0';

                    buf.ptr += snprintf(buf.ptr,1024,format,value);

                    Format(s, args...);
                    return ;
                case 's':
                case 'c':
                    s++;
                    Format(value);
                    Format(s, args...);
                    return;
                case '%':
                    *buf.ptr++= *s++;
                    Format(s,value,args...);
                    return ;
                default:
                    *ptr++ = *s++;
                }
            }
        }
    }

#elif __i386__
    void Format(const char *str,...)
    {
        va_list args;
        va_start(args,str);

        int len = snprintf(buf.ptr,buf.size,str,args);
        if(buf.size < len)
            return;
        buf.ptr  += len ;
        buf.size -= len;
        va_end(args);

    }
#endif

    template<typename T>
    LogEntry& operator<<(T t)
    {
        Format(t);
        return *this;
    }

    template<int N>
    LogEntry& operator<<(const char t[N])
    {

        Format((const char*)t);
        return *this;
    }


    LogEntry& operator<<(char t)
    {
        Format(t);
        return *this;
    }

    LogEntry& operator<<(const std::string& t)
    {
        Format(t.c_str());
        return *this;
    }


};


class CacheManger{
public:
    CacheManger();
    ~CacheManger();

    static LogEntry* mallocEntry();
    static void free(LogEntry* e);

    static LogBuffer* mallocBuffer(uint32_t size);
    static void free(LogBuffer* b);

public:
    SpinList<LogEntry*,SpinLock> _loglist;
    CacheManger * next;
};


class LogHelper{
public:
    static void OutPut(LogEntry* e);

public:
    static __thread uint32_t tid;
    static __thread char tidstr[8];
    static __thread time_t time;
    static __thread char datebuf[24];
};

class Worker;
//
class LogBacker {

public:

    ///
    /// \brief Init
    /// \param conf
    /// \return
    ///
    static int Init(Json::Value & conf);


    static void Stop(void *);
    static void Close();


    static void Join();

    ///
    /// \brief registerThread
    /// \param notify
    ///
    static void registerThread(CacheManger* cm);
    static void unregisterThread(CacheManger* cm);
    ///
    /// \brief OutPut
    /// \param e
    ///
    static void OutPut(LogEntry* e);


    ///
    /// \brief DumperLogger
    /// \param ctx
    ///
    static void DumperLogger(void *ctx);


    ///
    /// \brief DumperDefault
    /// \param ctx
    /// \param error
    /// \return
    ///
    static int DumperDefault(void *ctx,int error);

public:

    static int fd;
    static std::string logfile;
    static Worker* worker;
    static SpinList<LogEntry*,SpinLock> _loglist;
    static CacheManger* next;
    static MutexLock spin;
    static bool isruning;
};

}//namespace



#define DEBUG_FMG(format,args...) \
    if(DEBUG_LEVEL){ \
    ::mtrpc::LogEntry * e = ::mtrpc::CacheManger::mallocEntry();\
    e->Reset(DEBUG_LEVEL,__FILE__,__LINE__,__PRETTY_FUNCTION__); \
    e->Format(format,args); e->Format('\0');\
    ::mtrpc::LogHelper::OutPut(e); \
    };


#define TRACE_FMG(format,args...) \
    if(TRACE_LEVEL){ \
    ::mtrpc::LogEntry * e = ::mtrpc::CacheManger::mallocEntry();\
    e->Reset(TRACE_LEVEL,__FILE__,__LINE__,__PRETTY_FUNCTION__); \
    e->Format(format,args);e->Format('\0'); \
    ::mtrpc::LogHelper::OutPut(e); \
    };

#define INFO_FMG(format,args...) \
    if(INFO__LEVEL){ \
    ::mtrpc::LogEntry * e = ::mtrpc::CacheManger::mallocEntry();\
    e->Reset(INFO__LEVEL,__FILE__,__LINE__,__PRETTY_FUNCTION__); \
    e->Format(format,args);e->Format('\0'); \
    ::mtrpc::LogHelper::OutPut(e); \
    };

#define WARN_FMG(format,args...) \
    if(WARN__LEVEL){ \
    ::mtrpc::LogEntry * e = ::mtrpc::CacheManger::mallocEntry();\
    e->Reset(WARN__LEVEL,__FILE__,__LINE__,__PRETTY_FUNCTION__); \
    e->Format(format,args);e->Format('\0');\
    ::mtrpc::LogHelper::OutPut(e); \
    };


#define ERROR_FMG(format,args...) \
    if(ERROR_LEVEL){ \
    ::mtrpc::LogEntry * e = ::mtrpc::CacheManger::mallocEntry();\
    e->Reset(ERROR_LEVEL,__FILE__,__LINE__,__PRETTY_FUNCTION__); \
    e->Format(format,args);e->Format('\0'); \
    ::mtrpc::LogHelper::OutPut(e); \
    };



#define DEBUG(x) \
    if(DEBUG_LEVEL){ \
    ::mtrpc::LogEntry * e = ::mtrpc::CacheManger::mallocEntry();\
    e->Reset(DEBUG_LEVEL,__FILE__,__LINE__,__PRETTY_FUNCTION__); \
    (*e)<<x<<'\0'; \
    ::mtrpc::LogHelper::OutPut(e); \
    };


#define TRACE(x) \
    if(TRACE_LEVEL){ \
    ::mtrpc::LogEntry * e = ::mtrpc::CacheManger::mallocEntry();\
    e->Reset(TRACE_LEVEL,__FILE__,__LINE__,__PRETTY_FUNCTION__); \
    (*e)<<x<<'\0'; \
    ::mtrpc::LogHelper::OutPut(e); \
    };


#define INFO(x) \
    if(INFO__LEVEL){ \
    ::mtrpc::LogEntry * e = ::mtrpc::CacheManger::mallocEntry();\
    e->Reset(INFO__LEVEL,__FILE__,__LINE__, __PRETTY_FUNCTION__); \
    (*e)<<x<<'\0';  \
    ::mtrpc::LogHelper::OutPut(e); \
    };


#define WARN(x) \
    if(WARN__LEVEL){ \
    ::mtrpc::LogEntry * e = ::mtrpc::CacheManger::mallocEntry();\
    e->Reset(WARN__LEVEL,__FILE__,__LINE__,__PRETTY_FUNCTION__); \
    (*e)<<x<<'\0';  \
    ::mtrpc::LogHelper::OutPut(e); \
    };

#define ERROR(x) \
    if(ERROR_LEVEL){ \
    ::mtrpc::LogEntry * e = ::mtrpc::CacheManger::mallocEntry();\
    e->Reset(ERROR_LEVEL,__FILE__,__LINE__,__PRETTY_FUNCTION__); \
    (*e)<<x<<'\0'; \
    ::mtrpc::LogHelper::OutPut(e); \
    };



#define CHECK_LOG(x,m) \
    if(!(x)){WARN("check failed:"#x<<","#m":"<<m);}

#define SCHECK(expression) \
    if(!(expression)){WARN("check failed:"#expression);}
#endif
