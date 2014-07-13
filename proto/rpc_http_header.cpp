#include "rpc_http_header.h"
#include "mio/mio_error_code.h"
#include <sstream>
#include "log/log.h"
namespace mtrpc {


enum HTTP_PARSER_STATE
{
    START        = 0,
    END          = 1,
    // for REQ
    REQ_PATH     = 2,
    REQ_VESION_R = 3,
    REQ_VESION_N = 4,
    REQ_KEY      = 5,
    REQ_VALUE_R  = 6,
    REQ_VALUE_N  = 7,
    REQ_END_R    = 8,


    //for
    RES_STATUS           = 10,
    RES_STATUS_MESSAGE_R = 11,
    RES_STATUS_MESSAGE_N = 12,
    RES_KEY              = 13,
    RES_VALUE_R          = 14,
    RES_VALUE_N          = 15,
    RES_END_R            = 16,

};


HttpHeader::HttpHeader(){
    Reset();
}

void HttpHeader::Reset(){
    headers.clear();
    version="HTTP/1.1";

    state = START;
    memset(buf,0,sizeof(buf));
    pos = buf;
}

void HttpHeader::SetContentLength(uint32_t length)
{
    {
        static const std::string key = "Content-Length";
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "%u", length);
        headers.insert(std::make_pair(key, std::string(buf)));
    }

    {
        static const std::string key = "Content-Type";
        static const std::string value = "message/protobuf";
        headers.insert(std::make_pair(key, std::string(value)));
    }
}

int HttpHeader::GetContentLength(){

    const std::string key = "Content-Length";
    std::map<std::string,std::string>::iterator it = headers.find(key);
    if(it == headers.end())
        return -1;

    return strtol(it->second.c_str(),NULL,10);
}

void HttpHeader::SetSeq(uint64_t seq){

    static const std::string key = "Seq";
    char buf[32] = {0};
    snprintf(buf, sizeof(buf), "%lu", length);
    headers.insert(std::make_pair(key, std::string(buf)));
}


uint64_t HttpHeader::GetSeq(){

    const std::string key = "Seq";
    std::map<std::string,std::string>::iterator it = headers.find(key);
    if(it == headers.end())
        return -1;
    return strtoull(it->second.c_str(),NULL,10);

}



std::string HttpHeader::toString(){
    BufferPieces pi;

    IOBuffer::Iterator it;
    it._idx = it._pos = 0;
    it._que = (BufferPieces**)&pi;

    this->SerializeHeader(it);

    return std::string(pi.buffer);
}


bool HttpHeader::isParsed(){
    return state == END;
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


HttpRequestHeader::HttpRequestHeader():
    HttpHeader()
{
    method = "POST";
}


int HttpRequestHeader::ParserHeader(ReadBuffer & buf){

    ReadBuffer::Iterator& begin = buf.readpos;
    ReadBuffer::Iterator& end = buf.writepos;

    std::string key;
    std::string value;



    for(; begin!=end && state!= END ; ++begin)
    {

        char c = *begin;

        //onnly support the key size
        if((pos - buf) >= MAX_KEY_LEN)
            return HTTP_PARSER_FAIL;

        switch(state)
        {

        case START:
            switch(c)
            {
            case ' ':
                MoveBufTo(method);
                //only support POST
                if(strcmp(metod.c_str(),"POST") !=0 )
                    return HTTP_PARSER_FAIL;

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
                if(path.size() == 0)
                    return HTTP_PARSER_FAIL;

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
                state = END;
                bodyStart = begin;
                break;
            default:
                return HTTP_PARSER_FAIL;
            }
            break;
        }// switch

    }//end for

    return state == END  ? HTTP_PASER_FINISH : HTTP_PASER_HALF;
}

int HttpRequestHeader::SerializeHeader(WriteBuffer::Iterator& it){

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
    *bufptr = '\0';
    size -= ret;
    //trunctate the buffer
    it.get()->size = bufptr - bufpos;
    return true;
}



HttpReponseHeader::HttpReponseHeader():
    HttpHeader()
{

}

int HttpReponseHeader::ParserHeader(ReadBuffer & buf){

    ReadBuffer::Iterator& begin = buf.readpos;
    ReadBuffer::Iterator& end = buf.writepos;

    std::string key;
    std::string value;

    for(; begin!=end && state!= END; ++begin)
    {
        char c = *begin;

        switch(state)
        {
        case START:
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
                state = END;
                bodyStart = begin;
                break;
            default:
                return HTTP_PARSER_FAIL;
            }
            break;
        }//switch c
    }//end for



    TRACE("paser finished:"<<toString());
    return state ==END ? HTTP_PASER_FINISH : HTTP_PASER_HALF;
}


int HttpReponseHeader::SerializeHeader(WriteBuffer::Iterator& it){

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
    *bufptr = '\0';
    size -= ret;
    it.get()->size = bufptr -bufpos;
    return true;
}




}
