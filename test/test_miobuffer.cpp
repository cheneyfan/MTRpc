#include "gtest/gtest.h"

#include "stdio.h"
#include "mio2/mio_buffer.h"


using namespace mio2;





TEST(F1test,F1test1)
{

    MioBufferOut buf;

    char * p1 =new char[1024];
    char * p2 =new char[1024];
    char * p3 =new char[1024];
    char * p4 =new char[1024];

    buf.Append(p1,1024);

    char * res = NULL;
    uint32_t si = 0;

    buf.GetBuffer(100, res, si);

    ASSERT_EQ(1024,si);

    buf.Append(p2,1024);

    buf.GetBuffer(1025, res, si);

    ASSERT_EQ(2048,si)<<"test merge";

    std::vector<iovec> vec;
    buf.GetIovec(vec);

    ASSERT_EQ(1, vec.size())<<"test ioce";
}


TEST(F1test,F1test2)
{

    MioBufferIn buf;

    char * p1 = new char[1024];
    char * p2 = new char[1024];
    char * p3 = new char[1024];
    char * p4 = new char[1024];

    buf.Append(p1,2048);
    std::cout<<"buf.Append(p1,2048); h:"<<buf.HeadSize()<<",m:"<<buf.MidSize()<<",t:"<<buf.TailSize()<<std::endl;


    buf.WriteSkip(1024);
    std::cout<<"buf.WriteSkip(1024); h:"<<buf.HeadSize()<<",m:"<<buf.MidSize()<<",t:"<<buf.TailSize()<<std::endl;

    buf.ReadSkip(1024);

    std::cout<<"buf.ReadSkip(1024); h:"<<buf.HeadSize()<<",m:"<<buf.MidSize()<<",t:"<<buf.TailSize()<<std::endl;

    buf.Append(p1,1025);

    std::cout<<"buf.Append(p1,1025); h:"<<buf.HeadSize()<<",m:"<<buf.MidSize()<<",t:"<<buf.TailSize()<<std::endl;

    buf.Append(p1,1024);
    std::cout<<"buf.Append(p1,1024); h:"<<buf.HeadSize()<<",m:"<<buf.MidSize()<<",t:"<<buf.TailSize()<<std::endl;

}



int main(int argc,char*argv[]){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

