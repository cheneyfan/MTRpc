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
#define MAX_BUFFER_PIECES 64

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/uio.h>
#include <vector>


#include <google/protobuf/io/zero_copy_stream.h>

namespace mtrpc {

typedef  ::google::protobuf::int64 int64;

/// Every Buffer has same size
///
class BufferPieces
{
public:

    BufferPieces():
        buffer(NULL),
        size(DEFAULT_BUFFER_SIZE)
    {
    }

    ~BufferPieces()
    {
        delete[] buffer;
    }

public:
    char* buffer;
    /// the buffer current used max size;
    int size;
};


class IOBuffer
{
public:
    class Iterator
    {
    public:

        Iterator():
            _idx(0),_pos(0),_que(NULL){

        }

        Iterator(int idx,int pos,BufferPieces** que =NULL):
            _idx(idx),_pos(pos),_que(que)
        {
        }

        int operator - (const Iterator& it) const
        {
            //
            int size = 0;


            int end_idx =  _idx;
            int start_idx = it._idx;

            if(end_idx == start_idx && it._pos <= _pos)
            {
                return _pos - it._pos;
            }

            size += _que[it._idx]->size - it._pos;

            for(int idx  = start_idx ;
                idx != end_idx;
                idx  = (idx +1)%MAX_BUFFER_PIECES)
            {
                //map be some pieces not use whole
                size +=_que[idx%MAX_BUFFER_PIECES]->size;
            }

            size+=  _pos;
            return size;
        }

        Iterator& operator += (int size)
        {
            _pos += size;

            while(_pos >= _que[_idx]->size )
            {
                _pos -= _que[_idx]->size;
                _idx  = (_idx + 1)%MAX_BUFFER_PIECES;
            }

            return *this;
        }

        Iterator& operator -= (int size)
        {
            _pos -= size;
            while(_pos < 0 )
            {
                _pos += _que[_idx]->size;
                _idx = (_idx -1 + MAX_BUFFER_PIECES)%MAX_BUFFER_PIECES;
            }
            return *this;
        }

       char operator*(){
           return _que[_idx]->buffer[_pos];
       }

       BufferPieces* get(){
           return _que[_idx];
       }

       Iterator& operator ++ (){

           if(++_pos < _que[_idx]->size)
               return *this;

           _pos = 0;
           _idx = (_idx+1) % MAX_BUFFER_PIECES;
           return *this;
       }

       Iterator& operator ++ (int){
           if(++_pos < _que[_idx]->size)
               return *this;

           _pos = 0;
           _idx = (_idx+1) % MAX_BUFFER_PIECES;
           return *this;
       }

       bool operator == (Iterator& it){
           return (_idx == it._idx) && (_pos == it._pos);
       }

       bool operator !=(Iterator& it){
           return !( (_idx == it._idx) && (_pos == it._pos));
       }

    public:
       int _idx;
       int _pos;
       BufferPieces** _que;
    };

public:

    ///
    /// \brief IOBuffer
    ///
    IOBuffer();

    ///
    /// \brief ~IOBuffer
    ///
    virtual ~IOBuffer();


    Iterator& begin()
    {
        return readpos;
    }

    Iterator Reserve();

    Iterator& end()
    {
        return writepos;
    }

    //use keep readed buffer from socket
    bool GetReciveBuffer(struct iovec* iov,int& iov_num);
    bool MoveRecivePtr(int size);



    //use keep write buffer to socket
    bool GetSendBuffer(struct iovec* iov,int& iov_num);
    bool GetSendBuffer(struct iovec* iov, int& iov_num, const Iterator &it);
    bool MoveSendPtr(int size);

    //use size
    int GetBufferLeft();
    uint32_t GetBufferUsed();

    bool isFull();
    bool isEmpty();

public:
    BufferPieces** que;
    Iterator readpos;
    Iterator writepos;
};


class ReadBuffer : public IOBuffer,
                   public google::protobuf::io::ZeroCopyInputStream
{
public:

    ReadBuffer();

    // implements ZeroCopyInputStream
    bool Next(const void** data, int* size);
    void BackUp(int count);
    bool Skip(int count);
    int64 ByteCount() const;
    void beginPacket(){
        beginRead = readpos;
    }
    Iterator beginRead;
};

class WriteBuffer: public IOBuffer,
                   public google::protobuf::io::ZeroCopyOutputStream {
public:

    WriteBuffer();

    // implements ZeroCopyOutputStream ---------------------------------
    bool Next(void** data, int* size);
    void BackUp(int count);
    int64 ByteCount() const;

    void begin(){
        writeRead = readpos;
    }
    Iterator writeRead;

};



}
#endif
