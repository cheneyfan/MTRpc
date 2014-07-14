// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: <guofutan@tencent.com>
// Created: 2013-07-01
// Description:

#include "log/log.h"
#include "mio_buffer.h"


#define MOD_PIECES(x) ((x)%MAX_BUFFER_PIECES)

namespace mtrpc {

uint32_t DEFAULT_BUFFER_SIZE = 4096;
uint32_t MAX_BUFFER_PIECES = 64;


IOBuffer::IOBuffer():
    queNum(MAX_BUFFER_PIECES),
    que(NULL),
    readpos(0,0,NULL),
    writepos(0,0,NULL)

{
    //
    que = new BufferPieces*[MAX_BUFFER_PIECES];

    //fill the buffer
    for(uint32_t i=0; i<MAX_BUFFER_PIECES; ++i)
        que[i] = new BufferPieces();

    readpos._que = que;
    writepos._que = que;
}

IOBuffer::~IOBuffer(){
    for(uint32_t i=0; i<MAX_BUFFER_PIECES; ++i)
        delete que[i];
    delete que;
}

WriteBuffer::Iterator IOBuffer::AlignWritePos()
{

    que[writepos._idx]->size = writepos._pos;

    writepos._pos = 0;
    writepos._idx = MOD_PIECES(writepos._idx +1);

    return writepos;

}

bool IOBuffer::Reserve(uint32_t size){

    if(writepos._idx == readpos._idx
            && writepos._pos < readpos._pos)
    {
         return false;
    }

    if(que[writepos._idx]->size - writepos._pos < size )
    {
        AlignWritePos();
    }

    return true;
}



bool IOBuffer::GetReciveBuffer(struct iovec* iov,int& iov_num){

    if(isFull())
    {
        iov_num = 0;
        return false;
    }

    int idx = 0;

    int end_idx =  readpos._idx;
    int start_idx = writepos._idx;

    if(end_idx == start_idx && writepos._pos < readpos._pos)
    {

        iov[idx].iov_base = que[writepos._idx]->buffer + writepos._pos;
        iov[idx].iov_len  =  readpos._pos - writepos._pos;
        iov_num = idx + 1;
        return true;
    }


    iov[idx].iov_base = que[writepos._idx]->buffer + writepos._pos;
    iov[idx].iov_len  = que[writepos._idx]->size   - writepos._pos;


    for(int tidx  = MOD_PIECES(start_idx+1);
        tidx != end_idx;
        tidx  = MOD_PIECES(tidx +1))
    {
        ++idx;
        iov[idx].iov_base = que[tidx]->buffer;
        iov[idx].iov_len  = que[tidx]->size;
    }

    // end
    if(readpos._pos > 0)
    {
        ++idx;
        iov[idx].iov_base = que[readpos._idx]->buffer;
        iov[idx].iov_len  = readpos._pos;
    }

    iov_num = idx + 1;

    return true;
}


bool IOBuffer::MoveRecivePtr(int size){
    writepos  += size;
    return true;
}


bool IOBuffer::GetSendBuffer(struct iovec* iov, int& iov_num, const Iterator& end){

    if(end == readpos)
    {
        iov_num = 0;
        return false;
    }

    int idx = 0;

    int end_idx =  end._idx;
    int start_idx = readpos._idx;

    if(end_idx == start_idx && readpos._pos < end._pos)
    {

        iov[idx].iov_base = que[readpos._idx]->buffer + readpos._pos;
        iov[idx].iov_len  =  end._pos - readpos._pos;
        iov_num = idx +1;
        return true;
    }

    iov[idx].iov_base = que[readpos._idx]->buffer + readpos._pos ;
    iov[idx].iov_len  = que[readpos._idx]->size - readpos._pos;


    for(int tidx  = MOD_PIECES(start_idx+1);
        tidx != end_idx;
        tidx  = MOD_PIECES(tidx +1))
    {
        ++idx;
        iov[idx].iov_base = que[tidx]->buffer;
        iov[idx].iov_len    = que[tidx]->size;
    }

    if(end._pos > 0)
    {
        ++idx;
        iov[idx].iov_base = que[end._idx]->buffer;
        iov[idx].iov_len  = end._pos;
    }
    iov_num = idx +1;
    return true;
}


//use keep write buffer to socket
bool IOBuffer::GetSendBuffer(struct iovec* iov,int& iov_num){
    return GetSendBuffer(iov, iov_num, writepos);
}

bool IOBuffer::MoveSendPtr(int size){
    readpos += size;
    return true;
}


int IOBuffer::GetBufferLeft(){

    return readpos  - writepos;
}

uint32_t IOBuffer::GetBufferUsed(){

    return  writepos  - readpos;
}

bool IOBuffer::isFull(){

    return ( writepos._idx  == readpos._idx)
            && (writepos._pos +1) ==readpos._pos;

}

bool IOBuffer::isEmpty()
{
    return readpos == writepos;
}

ReadBuffer::ReadBuffer():
    IOBuffer()
{

}

ReadBuffer::ReadBuffer(const WriteBuffer &buf)
{
    this ->queNum = buf.queNum;
    this ->que = buf.que;
    this->readpos = buf.readpos;
    this->writepos = buf.writepos;
    this->beginRead = buf.beginWrite;
}



bool ReadBuffer::Next(const void** data, int* size)
{
    if(isEmpty())
        return false;

    if(readpos._idx == writepos._idx &&
            readpos._pos < writepos._pos)
    {
        *data = que[readpos._idx]->buffer + readpos._pos;
        *size = writepos._pos - readpos._pos;
        readpos._pos = writepos._pos;
        return true;
    }

    *data = que[readpos._idx]->buffer + readpos._pos;
    *size = que[readpos._idx]->size - readpos._pos;

    readpos._idx = MOD_PIECES(readpos._idx +1);
    readpos._pos =0;
    return true;
}

void ReadBuffer::BackUp(int count){
    readpos -= count;
}

bool ReadBuffer::Skip(int count){

    int tmp_idx = readpos._idx;
    int tmp_pos = readpos._pos + count;

    while(tmp_pos >= que[tmp_idx]->size )
    {
        tmp_pos -= que[tmp_idx]->size;
        tmp_idx = MOD_PIECES(tmp_idx +1);
    }

     readpos._idx = tmp_idx;
     readpos._pos = tmp_pos;
     return true;
}

int64 ReadBuffer::ByteCount() const{
    return readpos - beginRead;
}



// implements ZeroCopyOutputStream ---------------------------------

WriteBuffer::WriteBuffer():
    IOBuffer(){

}

WriteBuffer::WriteBuffer(const ReadBuffer &buf)
{
    this ->queNum = buf.queNum;
    this ->que = buf.que;
    this->readpos = buf.readpos;
    this->writepos = buf.writepos;
}

bool WriteBuffer::Next(void** data, int* size)
{

    if(isFull())
        return false;

    if(readpos._idx == writepos._idx &&
             writepos._pos < (readpos._pos -1))
    {

        *data = que[writepos._idx]->buffer + writepos._pos;
        *size = readpos._pos -1 - writepos._pos;
        writepos._pos = readpos._pos -1;
        return true;
    }

    *data = que[writepos._idx]->buffer + writepos._pos;
    *size = que[writepos._idx]->size - writepos._pos;

    writepos._idx = MOD_PIECES(writepos ._idx+1);
    writepos._pos =0;
    return true;

}
void WriteBuffer::BackUp(int count)
{
    writepos -= count;
}

int64 WriteBuffer::ByteCount() const {
    return writepos - beginWrite;
}

}


