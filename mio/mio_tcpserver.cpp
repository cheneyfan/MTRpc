#include "mio_tcpserver.h"

namespace mio2 {


TcpServer::~TcpServer(){

}



int TcpServer::Start(){

    group.Init(threadnum);

    Closure task = Closure::From<Epoller,&Epoller::poll>(&poll);

    group.Post(task);

    accepter.onAccept = onAccept;
    poll.addEvent(&accepter.ev,true,false);

    return accepter.startListen(host,port);

}


void TcpServer::Stop(){

    poll.DelEvent(&accepter.ev);
    poll.Stop();
    group.Stop();

}

void TcpServer::Join(){
    group.join();
}

}
