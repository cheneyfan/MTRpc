#ifndef __ATOMIC_H
#define __ATOMIC_H

#include <stdint.h>


template<typename T>
class AtomicIntegerT
{
    typedef AtomicIntegerT<T> self_type;

public:
    AtomicIntegerT()
        : value_(0)
    {
    }

    AtomicIntegerT(const self_type& other)
    {
        //self_type val = other;
        __sync_lock_test_and_set(&value_, other.value_);
    }

    self_type& operator = (const self_type& other)
    {
        if (this != &other)
        {
            //self_type val = other;
            __sync_lock_test_and_set(&value_, other.value_);
        }
        return *this;
    }

    operator T() const
   {
       return __sync_val_compare_and_swap(const_cast<volatile T*>(&value_),0,0);
   }

    T get()
    {
        return __sync_val_compare_and_swap(&value_, 0, 0);
    }

    T getAndAdd(T x)
    {
        return __sync_fetch_and_add(&value_, x);
    }

    T addAndGet(T x)
    {
        return getAndAdd(x) + x;
    }

    T incrementAndGet()
    {
        return addAndGet(1);
    }

    T decrementAndGet()
    {
        return addAndGet(-1);
    }

    void add(T x)
    {
        getAndAdd(x);
    }

    void increment()
    {
        incrementAndGet();
    }

    void decrement()
    {
        decrementAndGet();
    }

    T getAndSet(T newValue)
    {
        return __sync_lock_test_and_set(&value_, newValue);
    }

    bool compareAndSet(T oldvalue,T newValue){

        return __sync_bool_compare_and_swap(&value_, oldvalue, newValue);
    }

    void largerSet(T newValue)
    {
        T oldvalue;
        do
        {
           oldvalue = value_;

        }while(newValue > oldvalue
               && !__sync_bool_compare_and_swap(&value_,oldvalue,newValue));
    }



    void smallerSet(T newValue)
    {
        T oldvalue;
        do
        {
           oldvalue = value_;

        }while(newValue < oldvalue
               && __sync_val_compare_and_swap(&value_,oldvalue,newValue));
    }

private:
    volatile T value_;
};


typedef AtomicIntegerT<int32_t> AtomicInt32;
typedef AtomicIntegerT<int64_t> AtomicInt64;


#endif
