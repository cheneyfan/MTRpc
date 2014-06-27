// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: <guofutan@tencent.com>
// Created: 2013-07-01
// Description:

#include "log.h"

#include "mio_buffer.h"


namespace mio2 {


MioBufferIn::MioBufferIn():
    buffer(new char[DEFALUT_BUFFER_SIZE]),
    size(DEFALUT_BUFFER_SIZE),
    rptr(buffer),
    wptr(buffer)
{
}

MioBufferIn::~MioBufferIn(){
    delete [] buffer;
}

void MioBufferIn::WriteSkip(uint32_t num){

    wptr += num;
}

void MioBufferIn::ReadSkip(uint32_t num)
{
    rptr += num;
}

uint32_t MioBufferIn::MidSize(){
    return wptr - rptr;
}

uint32_t MioBufferIn::TailSize(){
    return size - (wptr - buffer);
}

uint32_t MioBufferIn::HeadSize(){
    return rptr - buffer;
}

void MioBufferIn::Append(char * buff,uint32_t num)
{
    //just append;
    if(TailSize() >= num )
    {
        memcpy(wptr,buff,num);
        wptr+=num;
        return;
    }

    //move buffer
    if(HeadSize() + TailSize() >= num)
    {
        memmove(buffer,rptr, MidSize());
        wptr =  buffer + (wptr -rptr);
        rptr = buffer;

        memmove(wptr,buff,num);
        wptr+= num;
        return;
    }

    while(size < (num + MidSize()) )
        size*=DEFALUT_BUFFER_RESIZE;

    //malloc bigsize;
    char * tmp = new char [size];
    memmove(tmp,rptr, MidSize());
    wptr  = tmp + (wptr -rptr);
    rptr =tmp;
    delete[]  buffer;
    buffer = tmp;
}


void MioBufferIn::clear(){
    delete buffer;
    buffer = NULL;
}


MioBufferOut::MioBufferOut():
    head(NULL),tail(NULL)
{
}

MioBufferOut::~MioBufferOut()
{
    clear();
}

void  MioBufferOut::Append(char* buf,uint32_t size)
{
    BufferPieces * bp = new BufferPieces(buf,size);
    if(!head)
    {
        head = bp;
        tail = bp;
    }else{
        tail->next = bp;
        tail= bp;
    }
}

void MioBufferOut::GetBuffer(uint32_t size,char* &resbuf,uint32_t & ressize)
{
    // if get the head
    if(size <= head->size)
    {
        resbuf = head->buffer;
        ressize = head->size;
        return;
    }


    BufferPieces* tmp = new BufferPieces();
    BufferPieces* ptr = head;

    while(tmp->size < size && ptr)
    {
        tmp->size += ptr->size;
        ptr= ptr->next;
    }

    tmp->buffer = new char[tmp->size];
    uint32_t tmpsize = 0;
    while(tmpsize < tmp->size && head)
    {
        memcpy(tmp->buffer+tmpsize,head->buffer,head->size);
        tmpsize += head->size;
        BufferPieces* back = head;
        head = head->next;
        delete back;
    }

    //append the first;
    tmp->next = head;
    head = tmp;

    //if size exceed the last pieces, reset the tail
    if(tmp->next == NULL)
        tail = head;

    resbuf = head->buffer;
    ressize = head->size;
}

void MioBufferOut::Consume(uint32_t size){

    // remove the whole buffer
    while(size >= head->size && head)
    {
        size -= head->size;
        BufferPieces* back = head;
        head = head->next;
        delete back;
    }

    //move to left
    if(size > 0 && head)
    {
        memmove(head->buffer,head->buffer+size, head->size - size);
        head->size = size;
    }

    // if head is last buffer
    if(head  && head->next == NULL)
        tail = head;

}

void MioBufferOut::GetIovec(std::vector<iovec> &vec)
{
    BufferPieces* ptr = head;

    while(ptr)
    {
        iovec v = {ptr->buffer,ptr->size};
        vec.push_back(v);
        ptr = ptr->next;
    }
}


bool MioBufferOut::Empty()
{
    return head !=NULL;
}


void MioBufferOut::clear(){

     while(head)
     {
         tail  = head;
         delete head;
         head = tail->next;
     }
     tail = NULL;
}
}
