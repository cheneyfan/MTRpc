#ifndef __RPC_HTTP_HEADER__
#define __RPC_HTTP_HEADER__

#include <string>
#include "mio/mio_buffer.h"
#include <map>
namespace mtrpc {

#define MAX_KEY_LEN 256
#define MAX_HEADER_SIZE 1024
#define MAX_METHOD_LEN 8

#define HTTP_METHOD_GET_S "GET "
#define HTTP_METHOD_POST_S "POST "

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


    virtual int SerializeBody(const std::string& body,
                              WriteBuffer::Iterator& it);

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


    void SetHeader(const std::string &key,const std::string &value)
    {
        headers[key] = value;
    }
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
    bool isMethodFound(char c);
    int GetPendSize();
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

    virtual void Reset();

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

    virtual void Reset();

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
