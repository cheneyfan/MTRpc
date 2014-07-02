#include <string>
#include "mio_buffer.h"
#include <map>
namespace mtrpc {


class HttpParser;
class HttpHeader {
public:
    HttpHeader();

    /// load
    bool ParserRequestHeader(ReadBuffer & buf);

    bool ParserReponseHeader(ReadBuffer & buf);


    bool SerializeReponseHeader(WriteBuffer& buf,WriteBuffer::Iterator& it);

    bool SerializeReponseHeader(WriteBuffer& buf,WriteBuffer::Iterator& it);

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
        std::string version;
    std::string content_type;
    uint32_t content_length;
    std::map<std::string,std::string> headers;

    uint32_t status;
};


}
