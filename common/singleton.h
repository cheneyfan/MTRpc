// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: <guofutan@tencent.com>
// Created: 2013-07-01
// Description:

#ifndef _SINGLETON_H_
#define _SINGLETON_H_



#include <pthread.h>

//for aexit
#include <stdlib.h>

namespace mtrpc {


/**
 * use pthread_once to implement singleton.
 */
template<typename T>
class Singleton
{
public:
    static T& instance()
    {
        pthread_once(&ponce_, &Singleton::init);
        return *value_;
    }

private:
    Singleton();
    ~Singleton();



    ///
    /// \brief init
    ///
    static void init()
    {
        value_ = new T();
        ::atexit(destroy);
    }

    ///
    /// \brief destroy the instance at program exit
    ///
    static void destroy()
    {
        typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1] __attribute__ ((unused));
        delete value_;
    }

private:
    static pthread_once_t ponce_;
    static T*             value_;
};


template<typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value_ = NULL;

/**
 * template class for threadlocal
 */
template<typename T>
class ThreadLocal 
{
public:

    ThreadLocal()
    {

        pthread_key_create(&pkey_, &ThreadLocal::destructor);
    }

    ~ThreadLocal()
    {
        pthread_key_delete(pkey_);
    }

    /**
     * @brief value
     * @return
     */
    T& value()
    {
        T* perThreadValue = static_cast<T*>(pthread_getspecific(pkey_));
        if (!perThreadValue) {
            T* newObj = new T();
            pthread_setspecific(pkey_, newObj);
            perThreadValue = newObj;
        }
        return *perThreadValue;
    }

    void set(T* newObj)
    {
        assert(pthread_getspecific(pkey_) == NULL);
        pthread_setspecific(pkey_, newObj);
    }

    bool isNull(){
        return static_cast<T*>(pthread_getspecific(pkey_)) ==NULL;
    }
private:

    /**
     * @brief destructor clear the destructor
     * @param x
     */
    static void destructor(void *x)
    {
        T* obj = static_cast<T*>(x);
        typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1] __attribute__ ((unused));
        delete obj;
    }

private:
    pthread_key_t pkey_;
};

/**
 * ThreadLocalSingleton every singleton
 */
template<typename T>
class ThreadLocalSingleton 
{
public:

    static T& instance()
    {
        if (!t_value_)
        {
            t_value_ = new T();
            deleter_.set(t_value_);
        }
        return *t_value_;
    }

    static T* pointer()
    {
        return t_value_;
    }



private:
    ThreadLocalSingleton();
    ~ThreadLocalSingleton();

    static void destructor(void* obj)
    {
        assert(obj == t_value_);
        typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1] __attribute__ ((unused));

        delete t_value_;
        t_value_ = 0;
    }


    static __thread T* t_value_;
    static ThreadLocal<T> deleter_;
};

template<typename T>
__thread T* ThreadLocalSingleton<T>::t_value_ = 0;

template<typename T>
ThreadLocal<T> ThreadLocalSingleton<T>::deleter_;

}
#endif
