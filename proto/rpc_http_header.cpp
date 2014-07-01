#include "rpc_http_header.h"

namespace mtrpc {


enum HTTP_PASER_STATE{
    START  = 0,
    METHOD = 1,
    PATH   = 2,
    KEY    = 3,
    VALUE  = 4,
    END    = 5,
};
enum TOKEN_TYPE {
    INVAILD = 0,
    BLANK   = 1,
    NEWLINE = 2,
};


static bool getNextToken(ReadBuffer &buf,IOBuffer::Iterator &it, char& c1,char& c2)
{
    if(it != buf.end())
    {
        c1 = *it;
        ++it;
    }else{
        return false;
    }

    if(c1='\r')
    {
        if(it != buf.end())
        {
            c1 = *it;
            ++it;
        }else {
            // save back '\r'
            --it;
            return false;
        }
    }

    return true;

}

class HttpParser
{
public :

    HttpParser(){
        state = START;
        tokentype = INVAILD;
    }


    int state;
    int tokentype;
    char buf[2048];
    int  pos;

    int reset();
    int Scan(char c1,char c2);

};


HttpHeader::HttpHeader(){

    parser = new HttpParser();
}

HttpHeader::ParserHeader(ReadBuffer &buf)
{

    while(getNextToken(buf,it,c1,c2,))
    {
        if(c1==' ')
            tokentype = BLANK;
        else if(c1=='\r' && c2=='\n')
            tokentype = NEWLINE;
        else
            tokentype = INVAILD;


        switch(state):
        {
         case START:

            break;
         case METHOD:
            break;

         case PATH:
            break;

         case KEY:
            break;
        }

    }



}


}
