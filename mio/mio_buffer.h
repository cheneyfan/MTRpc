// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: <guofutan@tencent.com>
// Created: 2013-07-01
// Description:

#ifndef _MIO_TCP_BUFFER_H_
#define _MIO_TCP_BUFFER_H_

#define DEFAULT_BUFFER_SIZE 4096
#define MAX_BUFFER_SIZE 4096*4096
#define MAX_BUFFER_PIECES 16

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/uio.h>
#include <vector>


#include <google/protobuf/io/zero_copy_stream.h>

namespace mtrpc {

class BufferPieces
{
public:

    BufferPieces():
        buffer(NULL),
        size(0)
    {
    }

    BufferPieces(uint32_t s):
        buffer(new char[s]),
        size(s)
    {
    }

    BufferPieces(char* buf,uint32_t s):
        buffer(buf),
        size(s)
    {
    }

    ~BufferPieces()
    {
        delete buffer;
    }

public:
    char* buffer;
    uint32_t size;
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

class IOBuffer
{
public:

    IOBuffer():
        readidx(0),
        readpos(0),
        writeidx(0),
        writepos(0)
    {

    }

    virtual ~IOBuffer(){}

    IOBuffer(uint32_t size):readidx(0),readpos(0),writeidx(0),writepos(0)
    {
        BufferPieces* p = new BufferPieces(size);
        que.reset(p);

    }

    //use read buffer from socket
    bool GetReciveBuffer(struct iovec* iov,int& iov_num);
    bool MoveRecivePtr(int size);



    //use write buffer to socket
    bool GetSendBuffer(struct iovec* iov,int& iov_num);
    bool MoveSendPtr(int size);

    uint32_t GetBufferLeft();
    uint32_t GetBufferUsed();

    // just keep the first buffer
    void Reset();

    //
    bool Extend(int radio);
public:

    class Iterator
    {
    public:



    public:
       int idx;
       int pos;
    };

    Iterator begin();
    Iterator end();
public:
    CircleQueue que;
    int readidx;
    int readpos;
    int writeidx;
    int writepos;



};


class ReadBuffer : public IOBuffer,
                   public google::protobuf::io::ZeroCopyInputStream
{
public:

    // implements ZeroCopyInputStream
    bool Next(const void** data, int* size);
    void BackUp(int count);
    bool Skip(int count);
    int64 ByteCount() const;
};

class WriteBuffer: public IOBuffer,
                   public google::protobuf::io::ZeroCopyOutputStream {
public:

    // implements ZeroCopyOutputStream ---------------------------------
    bool Next(void** data, int* size);
    void BackUp(int count);
    int64 ByteCount() const;

};



}
#endif
