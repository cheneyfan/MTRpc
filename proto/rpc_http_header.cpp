#include "rpc_http_header.h"
#include "mio/mio_error_code.h"
namespace mtrpc {


enum HTTP_REQ_STATE{
    START  = 0,
    METHOD = 1,
    PATH   = 2,
    VESION_R =3,
    VESION_N =4,
    KEY    = 5,
    VALUE_R  = 6,
    VALUE_N = 7,
    END_R    = 8,
    END_N   = 9,
};




class HttpParser
{
public :

    HttpParser(){
        state = START;

         memset(buf,0,2048);
         pos =buf;
    }

    void reset(){
        state = START;
         memset(buf,0,2048);
        pos = buf;
    }

    void MoveBufTo(std::string& s)
    {
        s.reserve(pos - buf);
        char * ptr = buf;
        while(*ptr++ ==' ');
        while(*ptr!= '\0')
        {
            s+=*ptr;
            *ptr='0';
            ptr++;
        }
        pos =buf;
    }

    int Parser(ReadBuffer::Iterator& begin,ReadBuffer::Iterator& end)
    {
        std::string key;
        std::string value;

        for(ReadBuffer::Iterator it = begin; it!=end; ++it)
        {
            char c = *it;

            switch(state)
            {

            case METHOD:

                switch(c)
                {
                 case ' ':
                    MoveBufTo(method);
                    parser->state = PATH;
                    break;
                 default:
                    *pos++ = c;
                    break;
                }

                break;
           case   PATH:

                switch(c)
                {
                 case ' ':
                    MoveBufTo(path);
                    parser->state = VESION_R;
                    break;
                 default:
                    *pos++ = c;
                    break;
                }

                break;
            case  VESION_R:

                switch(c)
                {
                 case '\r':
                    MoveBufTo(version);
                    parser->state = VESION_N;
                    break;
                 default:
                    *pos++ = c;
                    break;
                }
                 break;
            case  VESION_N:

                switch(c)
                {
                 case '\n':
                    parser->state = KEY;
                    break;
                 default:
                    return HTTP_PARSER_FAIL;
                }
                break;

            case  KEY:
                switch(c)
                {
                 case ':':
                    MoveBufTo(key);
                    parser->state = VALUE_R;
                    break;
                 case '\r':
                    parser->state = END_R;
                    break;
                 default:
                    *pos++ = c;
                    break;
                }
                break;
            case   VALUE_R:

                switch(c)
                {
                 case '\r':
                    MoveBufTo(value);
                    headers.insert(std::make_pair(key,value));

                    key.clear();
                    value.clear();
                    parser->state = VALUE_R;
                    break;
                 default:
                    *pos++ = c;
                    break;
                }
                break;
            case  VALUE_N:
                switch(c)
                {
                 case '\n':
                    parser->state = KEY;
                    break;

                 default:
                    return HTTP_PARSER_FAIL;
                }
                break;
            case   END_R:
                switch(c)
                {
                 case '\n':
                    parser->state = END_N;
                    break;
                 default:
                    return HTTP_PARSER_FAIL;
                }
                break;
            }// switch

        }//end for

        if(state == END_N )
            return state == END_N ? HTTP_PASER_FINISH : HTTP_PASER_HALF;
    }

    int state;
    char buf[2048];
    char* pos;
    int reset();
};


HttpHeader::HttpHeader(){
    parser = new HttpParser();
}

HttpHeader::ParserHeader(ReadBuffer &buf)
{

     ReadBuffer::Iterator& begin = buf.begin();
     ReadBuffer::Iterator& end = buf.end();





}


}
