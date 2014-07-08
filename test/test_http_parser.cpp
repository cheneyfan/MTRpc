#include "stdio.h"
#include <iostream>
#include "gtest/gtest.h"

#include "log/log.h"
#include "proto/rpc_http_header.h"

using namespace mtrpc;



TEST(HTTP_REQUST,REQ){
#define _1
#ifdef _1

    std::string s="POST /v2.api/?login HTTP/1.1\r\n            Host: passport.baidu.com\r\n    Connection: keep-alive \r\n           Content-Length: 670 \r\n             Cache-Control: max-age=0 \r\n          Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n       Origin: http://www.baidu.com \r\n             User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/29.0.1547.66 Safari/537.36 LBBROWSER \r\n            Content-Type: application/x-www-form-urlencoded \r\n            Referer: http://www.baidu.com/ \r\n            Accept-Encoding: gzip,deflate,sdch\r\n            Accept-Language: zh-N,zh;q=0.8\r\n\r\n";

    HttpHeader h;

    ReadBuffer b;
    b.que[0]->size = snprintf(b.que[0]->buffer,b.que[0]->size,"%s",s.c_str());

    ReadBuffer::Iterator head = b.Reserve();

    int ret = h.ParserRequestHeader(b);

    TRACE("ret:"<<ret<<",header:"<<h.toString());

    h.SerializeRequestHeader(b.writepos);

    TRACE("serial:"<<b.que[b.writepos._idx]->buffer);

#endif

}

TEST(HTTP_REQUST,RES)
{
    std::string s="HTTP/1.1 200 OK    \r\n             Pragma: no-cache\r\n          Expires: Wed, 19 Apr 2000 11:43:00 GMT\r\n           Last-Modified: Wed, 21 Jan 2004 19:51:30 GMT    \r\n            X-Content-Type-Options: nosniff    \r\n              Content-Type: image/gif    \r\n               Date: Fri, 20 Jun 2014 00:32:47 GMT    \r\n               Server: Golfe2    \r\n                  Content-Length: 35    \r\n                   Age: 1588123    \r\n         Cache-Control: private, no-cache, no-cache=Set-Cookie, proxy-revalidate    \r\n              Alternate-Protocol: 80:quic    \r\n             X-Cache: MISS from SK-SQUIDWALL-70    \r\n             X-Cache-Lookup: MISS from SK-SQUIDWALL-70:8080    \r\n              Via: 1.1 SK-SQUIDWALL-70:8080 (squid/2.7.STABLE6)\r\n\r\n";

    HttpHeader h;

    ReadBuffer b;
    b.que[0]->size = snprintf(b.que[0]->buffer,b.que[0]->size,"%s",s.c_str());

    ReadBuffer::Iterator head = b.Reserve();

    int ret = h.ParserReponseHeader(b);

    TRACE(s);
    TRACE("ret:"<<ret<<",header:"<<h.toString());

    h.SerializeReponseHeader(b.writepos);

    TRACE("serial:"<<b.que[b.writepos._idx]->buffer);

}


int main(int argc,char* argv[]){


    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

}
