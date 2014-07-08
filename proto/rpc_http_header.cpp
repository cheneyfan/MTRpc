#include "rpc_http_header.h"
#include "mio/mio_error_code.h"
#include <sstream>

namespace mtrpc {


enum HTTP_REQ_STATE{
    REQ_START    = 0,
    REQ_METHOD   = 1,
    REQ_PATH     = 2,
    REQ_VESION_R = 3,
    REQ_VESION_N = 4,
    REQ_KEY      = 5,
    REQ_VALUE_R  = 6,
    REQ_VALUE_N  = 7,
    REQ_END_R    = 8,
    REQ_END_N    = 9,
};


HttpHeader::HttpHeader(){
    Reset();
}

void HttpHeader::Reset(){
    method.clear();
    path.clear();
    content_type.clear();
    content_length = 0;
    headers.clear();
    status = 0;

    state = REQ_METHOD;
    memset(buf,0,2048);
    pos = buf;
}

void HttpHeader::MoveBufTo(std::string& s)
{
    s.reserve(pos - buf + 1);

    char * ptr = buf;

    while(*ptr ==' ') ptr++;

    while(*ptr!= '\0')
    {
        s += *ptr;
        *ptr = '\0';
         ptr++;
    }

    pos = buf;
}

void HttpHeader::MoveBufTo(uint32_t& s)
{

    char * ptr = buf;
    while(*ptr ==' ') ptr++;

    sscanf(ptr,"%u",&s);

    ptr = buf;
    while(*ptr != '\0'){
        *ptr='\0';
        ptr++;
    }

    pos=buf;
}

int HttpHeader::ParserRequestHeader(ReadBuffer & buf){

    ReadBuffer::Iterator& begin = buf.begin();
    ReadBuffer::Iterator& end = buf.end();

    std::string key;
    std::string value;

    for(; begin!=end; ++begin)
    {
        char c = *begin;
        //printf("%c",c);

        switch(state)
        {

        case REQ_METHOD:
            switch(c)
            {
            case ' ':
                MoveBufTo(method);
                state = REQ_PATH;
                break;
            default:
                *pos++ = c;
                break;
            }
            break;

        case REQ_PATH:

            switch(c)
            {
            case ' ':
                MoveBufTo(path);
                state = REQ_VESION_R;
                break;
            default:
                *pos++ = c;
                break;
            }

            break;
        case REQ_VESION_R:

            switch(c)
            {
            case '\r':
                MoveBufTo(version);
                state = REQ_VESION_N;
                break;
            default:
                *pos++ = c;
                break;
            }
            break;
        case REQ_VESION_N:

            switch(c)
            {
            case '\n':
                state = REQ_KEY;
                break;
            default:
                return HTTP_PARSER_FAIL;
            }
            break;

        case REQ_KEY:
            switch(c)
            {
            case ':':
                MoveBufTo(key);
                state = REQ_VALUE_R;
                break;
            case '\r':
                state = REQ_END_R;
                break;
            default:
                *pos++ = c;
                break;
            }
            break;
        case REQ_VALUE_R:

            switch(c)
            {
            case '\r':
                MoveBufTo(value);
                headers.insert(std::make_pair(key,value));
                key.clear();
                value.clear();
                state = REQ_VALUE_N;
                break;
            default:
                *pos++ = c;
                break;
            }
            break;
        case REQ_VALUE_N:
            switch(c)
            {
            case '\n':
                state = REQ_KEY;
                break;

            default:
                return HTTP_PARSER_FAIL;
            }
            break;
        case REQ_END_R:
            switch(c)
            {
            case '\n':
                state = REQ_END_N;
                break;
            default:
                return HTTP_PARSER_FAIL;
            }
            break;
        }// switch

    }//end for


    return state == REQ_END_N ? HTTP_PASER_FINISH : HTTP_PASER_HALF;
}


enum HTTP_RES_STATE{
    RES_START            = 0,
    RES_VESION           = 1,
    RES_STATUS           = 2,
    RES_STATUS_MESSAGE_R = 3,
    RES_STATUS_MESSAGE_N = 4,
    RES_KEY              = 5,
    RES_VALUE_R          = 6,
    RES_VALUE_N          = 7,
    RES_END_R            = 8,
    RES_END_N            = 9,
};

int HttpHeader::ParserReponseHeader(ReadBuffer & buf){

    ReadBuffer::Iterator& begin = buf.begin();
    ReadBuffer::Iterator& end = buf.end();

    std::string key;
    std::string value;

    for(ReadBuffer::Iterator it = begin; it!=end; ++it)
    {
        char c = *it;

        switch(state)
        {
        case RES_VESION:
            switch(c)
            {
            case ' ':
                MoveBufTo(version);
                state = RES_STATUS;
                break;
            default:
                *pos++ = c;
                break;
            }
            break;

        case RES_STATUS:

            switch(c)
            {
            case ' ':
                MoveBufTo(status);
                state = RES_STATUS_MESSAGE_R;
                break;
            default:
                *pos++ = c;
                break;
            }

            break;
        case RES_STATUS_MESSAGE_R:

            switch(c)
            {
            case '\r':
                MoveBufTo(status_meg);
                state = RES_STATUS_MESSAGE_N;
                break;
            default:
                *pos++ = c;
                break;
            }
            break;
        case RES_STATUS_MESSAGE_N:

            switch(c)
            {
            case '\n':
                state = RES_KEY;
                break;
            default:
                return HTTP_PARSER_FAIL;
            }
            break;

        case REQ_KEY:
            switch(c)
            {
            case ':':
                MoveBufTo(key);
                state = RES_VALUE_R;
                break;
            case '\r':
                state = RES_END_R;
                break;
            default:
                *pos++ = c;
                break;
            }
            break;
        case RES_VALUE_R:

            switch(c)
            {
            case '\r':
                MoveBufTo(value);
                headers.insert(std::make_pair(key,value));

                key.clear();
                value.clear();
                state = RES_VALUE_N;
                break;
            default:
                *pos++ = c;
                break;
            }
            break;
        case RES_VALUE_N:
            switch(c)
            {
            case '\n':
                state = RES_KEY;
                break;

            default:
                return HTTP_PARSER_FAIL;
            }
            break;
        case RES_END_R:
            switch(c)
            {
            case '\n':
                state = RES_END_N;
                break;
            default:
                return HTTP_PARSER_FAIL;
            }
            break;
        }//switch c
    }//end for

    return state == RES_END_N ? HTTP_PASER_FINISH : HTTP_PASER_HALF;
}


int HttpHeader::SerializeReponseHeader(WriteBuffer::Iterator& it){

    char * bufptr = it.get()->buffer + it._pos;
    int size      = it.get()->size;

    char * bufpos = bufptr;

    //head
    int ret = snprintf(bufptr,size,"%s %u %s\r\n"
                      ,version.c_str(),status,status_meg.c_str());

    bufptr+=ret;
    size -= ret;
    // key value

    for(std::map<std::string,std::string>::iterator it = headers.begin();
        it!=headers.end();++it)
    {
           std::string key = it->first;
           std::string value = it->second;

           ret = snprintf(bufptr,size,"%s: %s \r\n",key.c_str(),value.c_str());
           bufptr+=ret;
           size -= ret;
    }

    ret= snprintf(bufptr,size,"\r\n");
    bufptr+=ret;
    size -= ret;
    it.get()->size = bufptr -bufpos;
    return true;
}

int HttpHeader::SerializeRequestHeader(WriteBuffer::Iterator& it){

    char * bufptr = it.get()->buffer + it._pos;
    int size      = it.get()->size;

    char * bufpos = bufptr;

    //head
    int ret = snprintf(bufptr,size,"%s %s %s\r\n"
                      ,method.c_str(), path.c_str(), version.c_str());

    bufptr += ret;
    size   -= ret;
    // key value

    for(std::map<std::string,std::string>::iterator it = headers.begin();
        it!=headers.end();++it)
    {
           std::string key = it->first;
           std::string value = it->second;

           ret = snprintf(bufptr,size,"%s: %s \r\n",key.c_str(),value.c_str());
           bufptr+=ret;
           size -= ret;
    }

    //end
    ret= snprintf(bufptr,size,"\r\n");
    bufptr+=ret;
    size -= ret;
    it.get()->size = bufptr -bufpos;
    return true;
}


int HttpHeader::SetPath(const std::string& p)
{
    method="POST";
    path = "/"+p;
    version = "HTTP/1.1";
    return 0;
}

int HttpHeader::SetContentLength(uint32_t length)
{
    std::string key = "Content-Length";
    char buf[32] = {0};
    snprintf(buf, 32, "%u", length);

    headers.insert(std::make_pair(key, std::string(buf)));

    key = "Content-Type";
    std::string value = "message/protobuf";
    headers.insert(std::make_pair(key, std::string(value)));
    return 0;
}

int HttpHeader::SetRequestSeq(uint32_t seq){
    std::string key = "Cookie";
    char buf[32] = {0};
    snprintf(buf, 32, "%u", seq);
    headers.insert(std::make_pair(key, std::string(buf)));
    return 0;
}

int HttpHeader::SetResponseSeq(uint32_t seq){
    std::string key = "Set-Cookie";
    char buf[32] = {0};
    snprintf(buf, 32, "%u", seq);
    headers.insert(std::make_pair(key, std::string(buf)));
    return 0;
}

int HttpHeader::SetStatus(uint32_t s){

    status = s;
    status_meg ="OK";
    return 0;
}


std::string HttpHeader::toString(){

     std::stringstream str;

     str<<"method:"<<method<<",";
     str<<"path:"<<path<<",";
     str<<"version:"<<version<<",";
     str<<"content_type:"<<content_type<<",";
     str<<"content_length:"<<content_length<<",";
     str<<"status:"<<status<<",";
     str<<"status_meg:"<<status_meg<<",";

     for(std::map<std::string,std::string>::iterator it = headers.begin(); it !=headers.end();++it)
     {
         str<<it->first<<":"<<it->second<<",";
     }

     return str.str();
}


}
