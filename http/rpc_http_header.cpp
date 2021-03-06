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

static const std::string Content_Length_key = "Content-Length";
static const std::string Content_type_key = "Content-Type";
static const std::string Content_type_value = "message/protobuf";
static const std::string Seq_key = "Seq";

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
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "%u", length);
        headers[Content_Length_key]=std::string(buf);
    }
}

int HttpHeader::GetContentLength(){

    std::map<std::string,std::string>::iterator it = headers.find(Content_Length_key);
    if(it == headers.end())
        return 0;

    return strtol(it->second.c_str(), NULL, 10);
}

void HttpHeader::SetSeq(uint64_t seq){


    char buf[32] = {0};
    snprintf(buf, sizeof(buf), "%lu", seq);
    headers[Seq_key]=std::string(buf);
}



uint64_t HttpHeader::GetSeq(){


    std::map<std::string,std::string>::iterator it = headers.find(Seq_key);
    if(it == headers.end())
        return -1;
    return strtoull(it->second.c_str(),NULL,10);

}


bool HttpHeader::isMethodFound(char c){


    *pos = c;
    pos++;

    int size = pos - buf;

    if(size >= MAX_KEY_LEN)
    {
        memmove(buf, pos - MAX_METHOD_LEN, MAX_METHOD_LEN);
        pos = MAX_METHOD_LEN + buf;
    }


    int method_len = (sizeof(HTTP_METHOD_GET_S) -1);

    if(size >= method_len){
        // sizeof include the '\0'
        char *get_start = pos - method_len;

        if (memcmp(get_start, HTTP_METHOD_GET_S, method_len) == 0)
        {
            TRACE("find new method:"<<std::string(get_start, method_len)<<",size:"<<size);
            pos = buf;
            return true;
        }
    }

    method_len = (sizeof(HTTP_METHOD_POST_S) -1);
    if(size >= method_len){

        char *post_start = pos - method_len;
        if (memcmp(post_start, HTTP_METHOD_POST_S, method_len) == 0)
        {
            TRACE("find new method:"<<std::string(post_start, method_len)<<",size:"<<size);
            pos = buf;
            return true;
        }
    }

    return false;
}


int HttpHeader::SerializeBody(const std::string& body, WriteBuffer::Iterator& it ){

    char * bufptr = it.get()->buffer + it._pos;
    int size      = it.get()->size - it._pos;

    char * bufpos = bufptr;
    const char * body_start = body.c_str();
    const char * end = body_start + body.size();

    while(body_start < end)
    {
        int remian = end - body_start;

        if(remian <= size)
        {
            memcpy(bufptr, body_start, remian);
            body_start += remian;
            bufptr  += remian;

        }else{
            memcpy(bufptr, body_start, size);
            body_start += size;
            bufptr  += size;
            it._pos += bufptr;

            // move next buffer
            ++ it;
            bufptr = it.get()->buffer + it._pos;
            bufpos = bufptr;
            size = it.get()->size - it._pos;
        }
    }

    //update the pos
    it._pos = bufptr - bufpos;
    return 0;
}

std::string HttpHeader::toString(){
    BufferPieces* pi[1];
    BufferPieces  p;
    pi[0] = &p;

    IOBuffer::Iterator it;
    it._idx = it._pos = 0;
    it._que = (BufferPieces**)(&pi);

    this->SerializeHeader(it);

    return std::string(p.buffer);
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
    while(*ptr ==' ')
        ptr++;

    sscanf(ptr,"%u",&s);

    ptr = buf;
    while(*ptr != '\0'){
        *ptr='\0';
        ptr++;
    }

    pos = buf;
}

int HttpHeader::GetPendSize(){
   return pos - buf;
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

    // is a new parser

    for(; begin != end && state!= END; ++begin)
    {

        char c = *begin;
        int current_append_size = GetPendSize();
        
        if( current_append_size >= MAX_KEY_LEN )
        {
             WARN("Max Http Key is:"<<MAX_KEY_LEN<<",current:"<<current_append_size);
             return HTTP_PARSER_FAIL;
        }
        
        switch(state)
        {

        case START:
/*            switch(c)
            {
            case ' ':

                method.clear();
                MoveBufTo(method);
                //only support POST && GET
                //if(strcmp(method.c_str(),"POST") !=0 || )
                //    return HTTP_PARSER_FAIL;

                state = REQ_PATH;
                break;
            default:
                *pos++ = c;
                break;
            }
*/
            if(isMethodFound(c))
            {
                method.clear();
                MoveBufTo(method);
                state = REQ_PATH;
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
                version.clear();
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
                if(key.size() ==0)
                     return HTTP_PARSER_FAIL;

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
                headers[key] =value;
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
/*        case END:
            // find the 'POST space '
            // begin a new packet process
            if(isMethodFound(c))
            {
                state = REQ_PATH;
            }
            break;

*/

        }// switch


    }//end for

    return state == END  ? HTTP_PASER_FINISH : HTTP_PASER_HALF;
}


int HttpRequestHeader::SerializeHeader(WriteBuffer::Iterator& it){

    char * bufptr = it.get()->buffer + it._pos;
    int size      = it.get()->size  - it._pos;

    char * bufpos = bufptr;

    //head
    int ret = snprintf(bufptr,size,"%s %s %s\r\n"
                       ,method.c_str(), path.c_str(), version.c_str());

    bufptr += ret;
    size   -= ret;
    if(size <= 0)
    {
        return -1;
    }
    // key value

    for(std::map<std::string,std::string>::iterator it = headers.begin();
        it!=headers.end();++it)
    {
        std::string key = it->first;
        std::string value = it->second;

        ret = snprintf(bufptr,size,"%s: %s \r\n",key.c_str(),value.c_str());
        bufptr+=ret;
        size -= ret;
        if(size <= 0)
        {
            return -1;
        }
    }

    //end
    ret = snprintf(bufptr,size,"\r\n");
    bufptr+=ret;
    //*bufptr = '\0';
    size -= ret;

    if(size <= 0)
    {
        return -1;
    }

    //trunctate the buffer
    it.get()->size = bufptr - bufpos + it._pos;
    return 0;
}


void HttpRequestHeader::Reset(){

    HttpHeader::Reset();
    method="POST";
    path="";
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

                MoveBufTo(key);
                state = RES_STATUS;

                key.clear();
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
                MoveBufTo(status_msg);
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

        case RES_KEY:

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
                headers[key] =value;

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


    return state == END ? HTTP_PASER_FINISH : HTTP_PASER_HALF;
}


int HttpReponseHeader::SerializeHeader(WriteBuffer::Iterator& it){

    char * bufptr = it.get()->buffer + it._pos;
    int size      = it.get()->size - it._pos;

    char * bufpos = bufptr;

    //head
    int ret = snprintf(bufptr, size,"%s %u %s\r\n"
                       ,version.c_str(),status,status_msg.c_str());

    bufptr+=ret;
    size -= ret;
    // key value

    if(size <= 0)
    {
        return -1;
    }


    for(std::map<std::string,std::string>::iterator it = headers.begin();
        it!=headers.end();++it)
    {
        std::string key = it->first;
        std::string value = it->second;

        ret = snprintf(bufptr,size,"%s: %s \r\n",key.c_str(),value.c_str());
        bufptr += ret;
        size -= ret;

        if(size <= 0)
        {
            return -1;
        }

    }

    ret= snprintf(bufptr, size, "\r\n");
    bufptr += ret;
    //*bufptr = '\0';
    size -= ret;


    if(size <= 0)
    {
        return -1;
    }


    // this is the size in buffer piece
    it.get()->size = bufptr - bufpos + it._pos;

    return 0;
}





void HttpReponseHeader::Reset(){
    HttpHeader::Reset();
    status= 0;
    status_msg.clear();
}



}
