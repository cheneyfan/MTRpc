#ifndef _MIO_TASK_H_
#define _MIO_TASK_H_

#include "stdio.h"

namespace mtrpc {

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)
#define force_inline __attribute__((always_inline))

typedef void (*TaskHandler1)(void*);
typedef void (*TaskHandler2)(void*,void*);
typedef void (*TaskHandler3)(void*,void*,void*);

template<class To,class From>
To  AnyCast(From v) {

    union Tmp{
        To  a;
        From b;
    };

    Tmp tmp;
    tmp.b = v;

    return tmp.a;
}

///
/// \brief The Task struct
///
class Closure {
public:

    Closure():
        h1(NULL),ctx(NULL)
    {
    }

    ///p1
    template<class A>
    static Closure  From(TaskHandler1 handler,A arg){

        Closure c;
        c.h1  = AnyCast<void*>(handler);
        c.ctx = AnyCast<void*>(arg);
        return c;
    }

    template<class A,void(A::*f)(void)>
    static Closure From(A* a)
    {
        Closure c;
        c.h1 = (void*)(&TaskHelper<A,f>);
        c.ctx = (void*)a;
        return c;
    }

    template<class A,void(A::*f)(void)>
    static void TaskHelper(A* pa){
        ((A*)pa->*f)();
    }

    inline void operator()(){
        if(likely(h1))
        {
            ((TaskHandler1)h1)(ctx);
        }
    }


    //p2
    template<class A,class B,void(A::*f)(B)>
    static Closure From(A* a)
    {
        Closure c;
        c.h1 = (void*)(&TaskHelper<A,B,f>);
        c.ctx = a;
        return c;
    }

    template<class A,class B,void(A::*f)(B)>
    static void TaskHelper(void* pa,void* b){
        ((A*)pa->*f)(AnyCast<B>(b));
    }


    template<class B>
    inline void operator()(B b){
        if(likely(h1))
        {
            (AnyCast<TaskHandler2>(h1))(ctx, AnyCast<void*>(b));
        }
    }

    //p3
    template<class A,class B,class C,void(A::*f)(B,C)>
    static Closure From(A* a)
    {
        Closure c;
        c.h1 = (void*)(&TaskHelper<A,B,C,f>);
        c.ctx = a;
        return c;
    }

    template<class A,class B,class C,void(A::*f)(B,C)>
    static void TaskHelper(void* pa,void* b,void* c){
        ((A*)pa->*f)(AnyCast<B>(b), AnyCast<C>(c));
    }


    template<class B,class C>
    inline void operator()(B b,C c){
        if(likely(h1))
        {
            (AnyCast<TaskHandler3>(h1))(ctx, AnyCast<void*>(b),AnyCast<void*>(c));
        }
    }

public:
    void* h1;
    void* ctx;
};

///
/// \brief The ClosureP1 class
///
class ClosureP1: public Closure{
public:

    template<class A,class B, void(A::*f)(B b)>
    static ClosureP1 From(A* a, B b)
    {
        ClosureP1 c;
        c.h1  = (void*)(&TaskHelper<A,f>);
        c.ctx = (void*)a;
        c.p1  = (void*)b;
        return c;
    }

    template<class A,class B,void(A::*f)(B)>
    static void TaskHelper(void* pa,void* b){
        ((A*)pa->*f)(AnyCast<B>(b));
    }

    inline void operator()(){
        if(likely(h1))
        {
            ((TaskHandler2)h1)(ctx,p1);
        }
    }
public:
    void* p1;
};


}
#endif
