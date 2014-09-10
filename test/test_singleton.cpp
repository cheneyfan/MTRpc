#include <stdio.h>
#include <iostream>
#include <sys/time.h>


#include "log/log.h"

#include "thread/workpool.h"
#include "common/signalhelper.h"
#include "common/timerhelper.h"
#include "common/singleton.h"
using namespace mtrpc;

class A {

public:
    void hehe(){
        std::cout<<"heheA";
    }

};
class B{
public:
};
template<typename T>
class Singlton{

    class B: private T{
        friend class Singlton<T>;

    };
public:
    static const T& get(){
          static B t;
          return t;
    }
private:
    Singlton();
    ~Singlton();

};


union Entry{
 A* pa;
 B* pb;

};

int main(int argc,char* argv[]){
/*
    Singlton<A>* a = NULL;
    a->get().hehe();
*/
     Entry* vec = new Entry[10];
     for(int i=0;i<9;i++)
     {
         vec->pa = (A*)(i);
     }

     printf("vec:%x\n",vec);
     printf("&vec:%x\n",&vec);

     printf("vec+1:%x\n",vec+1);
     //printf("&vec+1:%x\n",&(vec+1));
     Entry*tmp1 = &vec[0];
     Entry*tmp2 = &vec[1];

     Entry *p = __sync_val_compare_and_swap(&tmp1,NULL,tmp2);

     printf("p:%x\n",p);
     printf("vec+1:%x\n",vec);
     //printf("&vec+1:%x\n",&(vec+1));

    return 0;
}
