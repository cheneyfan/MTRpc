#include "proto/face.pb.h"

#include <iostream>

using namespace google::protobuf;

int main(int argc,char* argv[]){


    /*const ::MethodDescriptor * mm = youtu::ImportServer::descriptor()->method(0);

    std::cout<<mm->full_name()<<std::endl;
    */

    youtu::ImportServer::descriptor();

    const ::MethodDescriptor * mds = DescriptorPool::generated_pool()->FindMethodByName("youtu.ImportServer.Import");

    if(mds)
        std::cout<<mds->full_name()<<std::endl;
    else
        std::cout<<"NULL"<<std::endl;


    const ::ServiceDescriptor* sds = DescriptorPool::generated_pool()->FindServiceByName("youtu.ImportServer");

    if(sds)
        std::cout<<sds->full_name()<<std::endl;
    else
        std::cout<<"NULL"<<std::endl;
}
