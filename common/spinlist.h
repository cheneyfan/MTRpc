#ifndef _SPIN_LIST_H_
#define _SPIN_LIST_H_

#include "rwlock.h"
#include <assert.h>
#define ASSERT_DEBUG(x)
#define MAX_CACHE_SIZE 1024

namespace mtrpc {


template<typename T,typename LOCK=SpinLock>
class SpinList
{

    struct Node {

        Node(T t):_t(t),next(NULL),pre(NULL)
        {

        }
        T _t;
        Node* next;
        Node* pre;
    };

public:
    SpinList():
        head(NULL),
        tail(NULL),
        _cache_size(0),
        cache(NULL)
    {

    }

    ~SpinList(){

        while(head)
        {
            Node* tmp = head->next;
            delete head;
            head = tmp;
        }

        while(cache){

            Node* tmp = cache->next;
            delete cache;
            cache = tmp;
        }
    }

    bool push(T t){

        WriteLock<LOCK> l(sl);

        Node * tmp;

        if(cache){

             tmp = cache;
             tmp->next =NULL;
             ASSERT_DEBUG(!tmp->pre);

             cache = cache->next;
             tmp->_t = t;
             --_cache_size;
             ASSERT_DEBUG(_cache_size >= 0);

        }else{
             tmp = new Node(t);
        }

        if(head == NULL)
        {
            ASSERT_DEBUG(!tail);
            head = tmp;
            tail = tmp;
        }else{
            tmp->next = head;
            head->pre = tmp;
            head = tmp;
            ASSERT_DEBUG(!head->pre);
        }

        return true;
    }


    bool pop(T &t)
    {
        WriteLock<LOCK> l(sl);

        if(tail == NULL)
        {
            ASSERT_DEBUG(!head);
            return false;
        }

        Node * pre = tail->pre;

        ASSERT_DEBUG(!tail->next);

        t = tail->_t;

        if(pre)
        {

            ASSERT_DEBUG(pre->next == tail);
            pre->next = NULL;
        }
        else
        {
            ASSERT_DEBUG(!pre);
            ASSERT_DEBUG(head == tail);
            head = NULL;
        }

        delete tail;
        tail = pre;
        return true;

/*
        if(_cache_size > 1024)
        {
            delete tail;
            tail = pre;
            return true;
        }


        tail->pre = NULL;
        tail->next = cache;
        cache = tail;
        _cache_size++;

        tail = pre;
*/
        return true;
    }

    bool empty(){
        WriteLock<LOCK> l(sl);
        return head == NULL;
    }

    void MoveTo(SpinList<T,LOCK> &list)
    {
        WriteLock<LOCK> l(sl);

        list.head = this->head;
        list.tail = this->tail;
        this->head = NULL;
        this->tail = NULL;
    }

    void Merge(SpinList<T,LOCK> &list){

         WriteLock<LOCK> l1(sl);
         WriteLock<LOCK> l2(list.sl);

         if(this->head == NULL)
         {
            this->head  = list.head;
            this->tail  = list.tail;

         }else{

             list.tail->next = head;
             head->pre = list.tail;
             head =list.head;
         }

         list.head = NULL;
         list.tail = NULL;
    }


private:
    Node* head;
    Node* tail;
    LOCK sl;

//
    int _cache_size;
    Node* cache;
};

}

#endif
