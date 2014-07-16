#ifndef __RPC_HTTP_HEADER__
#define __RPC_HTTP_HEADER__

#include <string>
#include "mio/mio_buffer.h"
#include <map>
namespace mtrpc {

#define MAX_KEY_LEN 256
#define MAX_HEADER_SIZE 1024
///
/// \brief The HttpHeader class
///
///  HttpHeader + protobuf body,
///  the content lenth is only the size of proto buf.
///
///
class HttpHeader{
public:

    HttpHeader();

    ///
    /// \brief Reset begin every parser
    ///
    virtual void Reset();

    ///
    /// \brief ParserHeader will move buf readpos.
    /// \param buf
    /// \return
    ///
    virtual int ParserHeader(ReadBuffer & buf)= 0;

    ///
    /// \brief SerializeHeader
    /// \param it
    /// \return
    ///
    virtual int SerializeHeader(WriteBuffer::Iterator& it) = 0;



    ///
    /// \brief SetContentLength
    /// \param length
    ///
    void SetContentLength(uint32_t length);

    int GetContentLength();



    ///
    /// \brief SetSeq a seq mark a request;
    /// \param seq
    ///
    void SetSeq(uint64_t seq);
    uint64_t GetSeq();


    ///
    /// \brief toString for test
    /// \return
    ///
    std::string toString();

    ///
    /// \brief isParsed the header is parser over
    /// \return
    ///
    bool isParsed();
public:
    /// helper the parser
    void MoveBufTo(std::string& s);
    void MoveBufTo(uint32_t& s);

    int header_size;
    int body_size;

public:
    int state;
    char buf[MAX_KEY_LEN];
    char* pos;

public:
    std::map<std::string,std::string> headers;
    IOBuffer::Iterator bodyStart;
    std::string version;
};

class HttpRequestHeader:public HttpHeader {
public:
    HttpRequestHeader();

    /// strict check for this come from diff client
    ///
    virtual int ParserHeader(ReadBuffer & buf);

    virtual int SerializeHeader(WriteBuffer::Iterator& it);


    void SetPath(const std::string p){
         path = p;
    }

public:
    std::string method;
    std::string path;
};


class HttpReponseHeader:public HttpHeader {
public:

    HttpReponseHeader();
    /// load
    ///
    virtual int ParserHeader(ReadBuffer & buf);
    virtual int SerializeHeader(WriteBuffer::Iterator& it);


    void SetStatus(uint32_t s,const std::string msg){
        status = s;
        status_msg = msg;
    }

public:

    uint32_t status;
    std::string status_msg;
};

}

#endif
