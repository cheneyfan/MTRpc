// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: <guofutan@tencent.com>
// Created: 2013-07-01
// Description:

#ifndef _MIO_TCP_BUFFER_H_
#define _MIO_TCP_BUFFER_H_



#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/uio.h>
#include <vector>


#include <google/protobuf/io/zero_copy_stream.h>

namespace mtrpc {

extern int32_t DEFAULT_BUFFER_SIZE;
extern uint32_t MAX_BUFFER_PIECES;


typedef  ::google::protobuf::int64 int64;

/// Every Buffer has same size
///
class BufferPieces
{
public:

    BufferPieces():
        buffer(new char[DEFAULT_BUFFER_SIZE]),
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

        Iterator(const Iterator& it):
            _idx(it._idx),
            _pos(it._pos),
            _que(it._que)
        {
        }

        Iterator(int idx,int pos,BufferPieces** que =NULL):
            _idx(idx),_pos(pos),_que(que)
        {
        }
        
        bool operator< (const Iterator& it) const
        {
            if(_idx < it._idx)
                return true;
            else if(_idx > it._idx)
                return false;
            
            return _pos < it._pos;
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

            for(int idx  = (start_idx +1)% MAX_BUFFER_PIECES;
                idx != end_idx;
                idx  = (idx +1)%MAX_BUFFER_PIECES)
            {
                //map be some pieces not use whole
                size +=_que[idx]->size;
            }

            size +=  _pos;
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

       bool operator == (const Iterator& it) const{
           return (_idx == it._idx) && (_pos == it._pos);
       }

       bool operator !=(const Iterator& it) const {
           return !( (_idx == it._idx) && (_pos == it._pos));
       }

       std::string toString(){
            char buf[32]={0};
            snprintf(buf,32,"[%d:%d]",_idx,_pos);
            return std::string(buf);
       }

       bool isBetween(const Iterator& A,const Iterator& B){

           if(*this == A)
               return true;
           if(*this == B)
               return false;

           if(A  < B  || A == B)
           {
               return A<*this &&  *this< B;
           }

           //B A
           return B<*this || A <*this;
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

    Iterator AlignWritePos();

    bool Reserve(uint32_t size);

    Iterator& end()
    {
        return writepos;
    }

    //use keep readed buffer from socket
    bool GetReciveBuffer(struct iovec* iov,int& iov_num);
    bool MoveRecivePtr(int size);



    //use keep write buffer to socket
    bool GetSendBuffer(struct iovec* iov,int& iov_num);
    //Send from readpost to end;
    bool GetSendBuffer(struct iovec* iov, int& iov_num, const Iterator &end);
    bool MoveSendPtr(int size);

    //use size
    int GetBufferLeft();
    uint32_t GetBufferUsed();

    bool isFull();
    bool isEmpty();

    void Reset();

public:
    uint32_t queNum;
    BufferPieces** que;

    Iterator readpos;
    Iterator writepos;
};


class WriteBuffer;

class ReadBuffer : public IOBuffer,
                   public google::protobuf::io::ZeroCopyInputStream
{
public:

    ReadBuffer();
    ReadBuffer(const WriteBuffer&);

    // implements ZeroCopyInputStream
    bool Next(const void** data, int* size);
    void BackUp(int count);
    bool Skip(int count);
    int64 ByteCount() const;
    void BeginPacket(){
        beginRead = readpos;
    }

    Iterator beginRead;
};

class WriteBuffer: public IOBuffer,
                   public google::protobuf::io::ZeroCopyOutputStream {
public:

    WriteBuffer();
    WriteBuffer(const ReadBuffer&);

    // implements ZeroCopyOutputStream ---------------------------------
    bool Next(void** data, int* size);
    void BackUp(int count);
    int64 ByteCount() const;

    void BeginPacket(){
        beginWrite = readpos;
    }
    Iterator beginWrite;

};



}
#endif
