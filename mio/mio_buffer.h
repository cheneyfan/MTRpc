// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: <guofutan@tencent.com>
// Created: 2013-07-01
// Description:

#ifndef _MIO_TCP_BUFFER_H_
#define _MIO_TCP_BUFFER_H_

#define DEFINE_BUFFER_SIZE 4096
#define MAX_BUFFER_SIZE 4096*4096
#define MAX_BUFFER_PIECES 32

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/uio.h>
#include <vector>

namespace mtrpc {





class BufferPieces
{
public:

    BufferPieces():
        buffer(NULL),
        size(0),
        readpos(0),
        writepos(0),
        pre(NULL),
        next(NULL)
    {
    }

    BufferPieces(uint32_t s):
        buffer(new char[s]),
        size(s),
        readpos(0),
        writepos(0),
        pre(NULL),
        next(NULL)
    {
    }

    BufferPieces(char* buf,uint32_t s):
        buffer(buf),
        size(s),
        readpos(0),
        writepos(0),
        pre(NULL),
        next(NULL)
    {
    }

    ~BufferPieces()
    {
        delete buffer;
    }
public:

    char* buffer;
    uint32_t size;
    uint32_t readpos;
    uint32_t writepos;
};

class CircleQueue
{
public:

    CircleQueue()
    {

        memset(que,0,sizeof(que));
        head = tail =  0;
    }

    ~CircleQueue()
    {

        clear();
    }

    BufferPieces * Head(){
        return que[head];
    }

    BufferPieces * Tail(){
        return que[tail];
    }

    BufferPieces * Get(int i){
       return que[i];
    }

    unsigned short PiecesSize(){
         return (head + MAX_BUFFER_PIECES-tail) % MAX_BUFFER_PIECES ;
    }

    bool IsFull()
    {
        return ((head + 1)% MAX_BUFFER_PIECES) == tail;
    }

    bool IsEmpty()
    {
        return head  == tail;
    }

    bool clear(){

         BufferPieces* p = NULL;
         while(PopTail(p))
         {
             delete p;
         }
    }

    bool reset(BufferPieces* p){

        BufferPieces* tmp;
        while(PopTail(tmp))
        {
            if(tmp == p)
                continue;
            delete tmp;
        }

        que[0] = p;
        head =tail = 0;

        return true;

    }

    bool PushHead(BufferPieces* p)
    {
        if(IsFull())
            return false;

        head = (head +1 )% MAX_BUFFER_PIECES;
        que[head] = p;
        return true;
    }


    bool PopTail(BufferPieces* &p)
    {
        if(IsEmpty())
            return false;

        p = que[tail];
        que[tail] = NULL;

        tail = (tail +1 )% MAX_BUFFER_PIECES;

        return true;
    }
public:
    BufferPieces* que[MAX_BUFFER_PIECES];
    unsigned short head;
    unsigned short tail;

};

class ReadBuffer : public google::protobuf::io::ZeroCopyInputStream
{
public:

    ReadBuffer();

    virtual ~ReadBuffer(){}

    ReadBuffer(uint32_t size)
    {
        BufferPieces* p = new BufferPieces(size);
        que.reset(p);
    }



    // implements ZeroCopyInputStream
    bool Next(const void** data, int* size);
    void BackUp(int count);
    bool Skip(int count);
    int64 ByteCount() const;

    void Reset();

public:

    CircleQueue que;

};

class MioBufferOut
{

public:

    MioBufferOut();

    ~MioBufferOut();
    ///
    /// \brief Append
    /// \param buf
    /// \param size
    ///
    void Append(char* buf,uint32_t size);


    ///
    /// \brief GetBuffer
    /// \param size
    /// \param resbuf
    /// \param ressize
    ///
    void GetBuffer(uint32_t size, char *&resbuf, uint32_t & ressize);


    ///
    /// \brief Consume
    /// \param size
    ///
    void  Consume(uint32_t size);

    ///
    /// \brief GetIovec
    /// \param vec
    ///
    void GetIovec(std::vector<iovec> &vec);

    ///
    /// \brief empty
    /// \return
    ///
    bool Empty();

    ///
    /// \brief clear
    ///
    void clear();
public:
    BufferPieces* head;
    BufferPieces* tail;
};


class ReadBuffer {
public:

};

}
#endif
