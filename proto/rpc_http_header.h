#ifndef __RPC_HTTP_HEADER__
#define __RPC_HTTP_HEADER__

#include <string>
#include "mio/mio_buffer.h"
#include <map>
namespace mtrpc {



class HttpHeader {
public:
    HttpHeader();

    /// load
    int ParserRequestHeader(ReadBuffer & buf);
    int ParserReponseHeader(ReadBuffer & buf);


    int SerializeRequestHeader(WriteBuffer::Iterator& it);
    int SerializeReponseHeader(WriteBuffer::Iterator& it);


    int SetPath(const std::string &p);

    int SetContentLength(uint32_t length);
    int GetContentLength();
    int SetRequestSeq(uint32_t seq);
    int SetResponseSeq(uint32_t seq);
    int SetStatus(uint32_t status);
public:
    bool isReqParsed();
    bool isResParsed();
public:
    void Reset();
    void MoveBufTo(std::string& s);
    void MoveBufTo(uint32_t& s);
    std::string toString();
public:
    int state;
    char buf[2048];
    char* pos;
public:
    std::string method;

    std::string path;
    std::string version;
    std::string content_type;
    uint32_t content_length;
    std::map<std::string,std::string> headers;

    uint32_t status;
    std::string status_meg;

    IOBuffer::Iterator bodyStart;
};


}

#endif
