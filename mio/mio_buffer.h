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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/uio.h>
#include <vector>

namespace mio2 {


#define DEFALUT_BUFFER_SIZE 4096
#define DEFALUT_BUFFER_RESIZE 2



///
/// \brief The MioBufferIn class
///
///  |**head**|**mid**|**tail**|
///  buffer   rptr    wptr     size
///
class MioBufferIn
{
public:

    ///
    /// \brief MioBufferIn
    ///
    MioBufferIn();

    ~MioBufferIn();

    ///
    /// \brief WriteSkip
    /// \param num
    ///
    void WriteSkip(uint32_t num);

    ///
    /// \brief ReadSkip
    /// \param num
    ///
    void ReadSkip(uint32_t num);

    ///
    /// \brief MidSize
    /// \return
    ///
    uint32_t MidSize();

    ///
    /// \brief TailSize
    /// \return
    ///
    uint32_t TailSize();

    ///
    /// \brief HeadSize
    /// \return
    ///
    uint32_t HeadSize();

    ///
    /// \brief Append
    /// \param buff
    /// \param num
    ///
    void Append(char * buff,uint32_t num);


    ///
    /// \brief clear
    ///
    void clear();

public:

    char* buffer;
    uint32_t size;

    char* rptr;
    char* wptr;
};



class BufferPieces
{
public:

    BufferPieces():
        buffer(NULL),size(0),next(NULL){

    }

    BufferPieces(char* buf,uint32_t s):
        buffer(buf),size(s),next(NULL){

    }

    ~BufferPieces(){
        delete buffer;
    }

    char * buffer;
    uint32_t size;
    BufferPieces* next;
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


}
#endif
