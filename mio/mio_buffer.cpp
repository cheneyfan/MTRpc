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


IOBuffer::IOBuffer():
    readpos(0,0,NULL),
    writepos(0,0,NULL)
{
    //
    que = new BufferPieces*[MAX_BUFFER_PIECES];

    //fill the buffer
    for(int i=0; i<MAX_BUFFER_PIECES; ++i)
        que[i] = new BufferPieces(DEFAULT_BUFFER_SIZE);

    readpos._que = que;
    writepos._que = que;
}

virtual IOBuffer::~IOBuffer(){
    for(int i=0; i<MAX_BUFFER_PIECES; ++i)
        delete que[i];
    delete que;
}


IOBuffer::Iterator IOBuffer::Reserve(){

    if(writepos._pos > 0)
    {
        que[writepos._idx]->size = writepos._pos;
        writepos._idx = MOD_PIECES(writepos._idx + 1);
        writepos._pos = 0;
    }

    Iterator it = writepos;

    writepos._idx = MOD_PIECES(writepos._idx +1);

    return it;
}

bool IOBuffer::GetReciveBuffer(struct iovec* iov,int& iov_num){

    if(isFull())
        return false;

    int idx = 0;

    int end_idx =  readpos._idx;
    int start_idx = writepos._idx;

    if(end_idx == start_idx && writepos._pos <= readpos._pos)
    {

        iov[idx].iov_base = que[writepos._idx]->buffer + writepos._pos;
        iov[idx].iov_len  =  readpos._pos - writepos._pos -1;
        iov_num = idx +1;
        return true;
    }


    iov[idx].iov_base = que[writepos._idx]->buffer + writepos._pos;
    iov[idx].iov_len  = que[writepos._idx]->size   - writepos._pos;


    for(int tidx  = start_idx ;
        tidx != end_idx;
        tidx  = (tidx +1)%MAX_BUFFER_PIECES)
    {
        ++idx;
        iov[idx].iov_base = que[tidx]->buffer;
        iov[idx].iov_len    = que[tidx]->size;
    }

    if(readpos._pos > 0)
    {
        ++idx;
        iov[idx].iov_base = que[readpos._idx]->buffer;
        iov[idx].iov_len  = readpos._pos -1;
    }


    return true;
}


bool IOBuffer::MoveRecivePtr(int size){
    writepos  += size;
    return true;
}



//use keep write buffer to socket
bool IOBuffer::GetSendBuffer(struct iovec* iov,int& iov_num){

    if(isEmpty())
        return false;

    int idx = 0;

    int end_idx =  writepos._idx;
    int start_idx = readpos._idx;

    if(end_idx == start_idx && readpos._pos <= writepos._pos)
    {

        iov[idx].iov_base = que[readpos._idx]->buffer + readpos._pos;
        iov[idx].iov_len  =  writepos._pos - readpos._pos;
        iov_num = idx +1;
        return true;
    }

    iov[idx].iov_base = que[start_idx._idx]->buffer + readpos._pos ;
    iov[idx].iov_len  = que[start_idx._idx]->size - readpos._pos;


    for(int tidx  = start_idx ;
        tidx != end_idx;
        tidx  = (tidx +1)%MAX_BUFFER_PIECES)
    {
        ++idx;
        iov[idx].iov_base = que[tidx]->buffer;
        iov[idx].iov_len    = que[tidx]->size;
    }

    if(writepos._pos > 0)
    {
        ++idx;
        iov[idx].iov_base = que[writepos._idx]->buffer;
        iov[idx].iov_len  = writepos._pos;
    }


    return true;
}

bool IOBuffer::MoveSendPtr(int size){
    readpos += size;
    return true;
}


int IOBuffer::GetBufferLeft(){

    return readpos  - writepos;
}

int IOBuffer::GetBufferUsed(){

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

bool ReadBuffer::Next(const void** data, int* size)
{
    if(isEmpty())
        return false;

    if(readpos._idx == writepos._idx &&
            readpos._pos < writepos._pos)
    {
        *data = que[readpos._idx]->buffer + readpos._pos;
        *size = writepos._pos - readpos._pos;
        return true;
    }

    *data = que[readpos._idx]->buffer + readpos._pos;
    *size = que[readpos._idx]->size - readpos._pos;
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
        bool isempty = (tmp_idx == writepos._idx)
                && (tmp_pos >= writepos._pos);

        if(isempty)
            return false;

        tmp_pos -= que[tmp_idx]->size;
        tmp_idx = MOD_PIECES(tmp_idx +1);
    }

     readpos._idx = tmp_idx;
     readpos._pos = tmp_pos;
     return true;
}

int64_t ReadBuffer::ByteCount(){
    return readpos - beginRead();
}



// implements ZeroCopyOutputStream ---------------------------------
bool WriteBuffer::Next(void** data, int* size)
{

    if(isFull())
        return false;

    if(readpos._idx == writepos._idx &&
             writepos._pos < readpos._pos -1 )
    {

        *data = que[writepos._idx]->buffer + writepos._pos;
        *size =  readpos._pos  - writepos._pos -1;
        return true;
    }

    *data = que[writepos._idx]->buffer + writepos._pos;
    *size = que[writepos._idx]->writepos - writepos._pos;
    return true;

}
void WriteBuffer::BackUp(int count)
{
    writepos -= count;
}

int64_t WriteBuffer::ByteCount(){
    return writepos - writeRead;
}

}


