#ifndef _MIO_CONNECTION_HANDLER_H_
#define _MIO_CONNECTION_HANDLER_H_

#include "mio2/mio_buffer.h"

namespace mio2 {

class ConnectHandler
{
public:


    /**
     * @brief GetHeaderLen
     * @return
     */
    int GetHeadLen(MioBufferIn& in){
        return 0;
    }


    /**
     * @brief GetBodyLen
     * @param packet
     * @param size
     * @return
     */
    int GetBodyLen(MioBufferIn& in){
        return 0;
    }

    /**
     * @brief OnPacket
     * @param packet
     * @param intput
     * @param outbuffer
     * @return
     */
    int OnPacket(MioBufferIn& in,MioBufferOut& out)
    {

        return 0;
    }



    /**
     * @brief OnConnect
     * @return
     */
    int OnConnect(int sockfd, MioBufferOut& out){
        return 0;
    }

    /**
     * @brief onClose
     * @return
     */
    int onClose(int sockfd)
    {
        return 0;

    }
};



}
#endif
