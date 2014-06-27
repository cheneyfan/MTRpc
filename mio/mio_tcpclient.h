#include "mio_event.h"
#include <list>

#include "mio_connection.h"
#include "mio_poller.h"

namespace  mio2 {

/**
 * @brief The TcpClient class
 */
class TcpClient
{
public:

    void Init(const char* host,uint32_t port)
    {

    }

    void AyncCall(){

        // new connect

        // add poll

        Connection * c = new Connection();


    }

    void Wait()
    {
        // poll

        // check whether all connect done or time out;
    }


public:
    const char* host;
    uint32_t port;
    Epoller poll;
    std::map<int,Connection* > connects;
};

}
