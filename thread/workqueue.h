#ifndef _MTRPC_WORK_QUEUE_H_
#define _MTRPC_WORK_QUEUE_H_

#include"common/rwlock.h"
#include "common/atomic.h"
namespace mtrpc {


template<class T,class Lock=MutexLock>
class WorkQueue{

    struct Node
    {
        Node():
            next(NULL)
        {
        }

        Node* next;
        T _t;
    };

public:
    WorkQueue()
    {
        sential = new Node();
        head = sential;
        tail = sential;
    }

    ~WorkQueue(){

        while(head)
        {
            Node* tmp =head;

            head=head->next;
            delete tmp;
        }
    }

    bool push(T& t)
    {
        Node *n = new Node();
        n->_t =t;

        {
            WriteLock<Lock> _wl(pushlock);
            tail->next= n;
            tail = n;
        }
        return true;
    }

    bool pop(T& t)
    {
        Node* node = NULL;
        {
            WriteLock<Lock> _wl(poplock);
            node = head;
            Node* new_head = node->next;

            if(NULL == new_head)
            {
                return false;
            }

            t= new_head->_t;

            head = new_head;
        }
        //free
        delete node;
        return true;
    }


public:
    Node* sential;
    Node* head;
    Node* tail;

    Lock pushlock;
    Lock poplock;
};
}
#endif
