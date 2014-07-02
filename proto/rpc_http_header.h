#include <string>
#include "mio_buffer.h"
#include <map>
namespace mtrpc {


class HttpParser;
class HttpHeader {
public:
    HttpHeader();

    /// load
    bool ParserHeader(ReadBuffer & buf);
    bool ParserBody(ReadBuffer & buf, Message* req);

    bool SerializeHeader(WriteBuffer& buf,WriteBuffer::Iterator& it);
    bool SerializeBody(WriteBuffer& buf, Message* res);

    void clear(){
        parser->reset();
        method.clear();
        path.clear();
        content_type.clear();
        content_length = 0;
        headers.clear();
        status = 0;
    }

public:
    HttpParser* parser;

public:
    std::string method;
    std::string path;
    std::string content_type;
    uint32_t content_length;
    std::map<std::string,std::string> headers;

    uint32_t status;
};


}
