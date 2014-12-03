#include "gtest/gtest.h"

#include "stdio.h"
#include "mio/mio_buffer.h"
#include "log/log.h"


using namespace mtrpc;


std::string toString(const IOBuffer::Iterator& it){

    char buf[32]={0};
    snprintf(buf,32,"%d_%d",it._idx,it._pos);
    return std::string(buf);
}

TEST(IOBUFFER,IOBUFFER_READ )
{
#ifdef _1
    mtrpc::DEFAULT_BUFFER_SIZE =8;
    mtrpc::MAX_BUFFER_PIECES = 3;
    IOBuffer b;





    struct iovec vec[3];
    int iov_num=3;
    memset(vec,0,sizeof(vec));

    b.GetReciveBuffer(vec, iov_num);

    TRACE("b size:"<<b.queNum<<",rit:"<<toString(b.begin())<<",wit:"<<toString(b.end()));
    TRACE("iov_num:"<<iov_num<<"|0:"<<vec[0].iov_base<<","<<vec[0].iov_len<<"|1:"<<vec[1].iov_base<<","<<vec[1].iov_len<<"|2:"<<vec[2].iov_base<<","<<vec[2].iov_len);

    b.que[0]->size = 6;
    b.que[1]->size = 7;
    b.que[2]->size = 2;

    memset(vec,0,sizeof(vec));
    b.GetReciveBuffer(vec, iov_num);

    TRACE("b size:"<<b.queNum<<",rit:"<<toString(b.begin())<<",wit"<<toString(b.end()));
    TRACE("iov_num:"<<iov_num<<"|0:"<<vec[0].iov_base<<","<<vec[0].iov_len<<"|1:"<<vec[1].iov_base<<","<<vec[1].iov_len<<"|2:"<<vec[2].iov_base<<","<<vec[2].iov_len);

    b.MoveRecivePtr(3);
    memset(vec,0,sizeof(vec));
    b.GetReciveBuffer(vec, iov_num);

    TRACE("b size:"<<b.queNum<<",rit:"<<toString(b.begin())<<",wit"<<toString(b.end()));
    TRACE("iov_num:"<<iov_num<<"|0:"<<vec[0].iov_base<<","<<vec[0].iov_len<<"|1:"<<vec[1].iov_base<<","<<vec[1].iov_len<<"|2:"<<vec[2].iov_base<<","<<vec[2].iov_len);


    b.MoveRecivePtr(4);
    memset(vec,0,sizeof(vec));
    b.GetReciveBuffer(vec, iov_num);

    TRACE("b size:"<<b.queNum<<",rit:"<<toString(b.begin())<<",wit"<<toString(b.end()));
    TRACE("iov_num:"<<iov_num<<"|0:"<<vec[0].iov_base<<","<<vec[0].iov_len<<"|1:"<<vec[1].iov_base<<","<<vec[1].iov_len<<"|2:"<<vec[2].iov_base<<","<<vec[2].iov_len);



    b.MoveRecivePtr(6);
    memset(vec,0,sizeof(vec));
    b.GetReciveBuffer(vec, iov_num);

    TRACE("b size:"<<b.queNum<<",rit:"<<toString(b.begin())<<",wit"<<toString(b.end()));
    TRACE("iov_num:"<<iov_num<<"|0:"<<vec[0].iov_base<<","<<vec[0].iov_len<<"|1:"<<vec[1].iov_base<<","<<vec[1].iov_len<<"|2:"<<vec[2].iov_base<<","<<vec[2].iov_len);

    b.MoveRecivePtr(1);
    memset(vec,0,sizeof(vec));
    b.GetReciveBuffer(vec, iov_num);

    TRACE("b size:"<<b.queNum<<",rit:"<<toString(b.begin())<<",wit"<<toString(b.end()));
    TRACE("iov_num:"<<iov_num<<"|0:"<<vec[0].iov_base<<","<<vec[0].iov_len<<"|1:"<<vec[1].iov_base<<","<<vec[1].iov_len<<"|2:"<<vec[2].iov_base<<","<<vec[2].iov_len);


    b.MoveSendPtr(1);
    memset(vec,0,sizeof(vec));
    b.GetReciveBuffer(vec, iov_num);

    TRACE("b size:"<<b.queNum<<",rit:"<<toString(b.begin())<<",wit"<<toString(b.end()));
    TRACE("iov_num:"<<iov_num<<"|0:"<<vec[0].iov_base<<","<<vec[0].iov_len<<"|1:"<<vec[1].iov_base<<","<<vec[1].iov_len<<"|2:"<<vec[2].iov_base<<","<<vec[2].iov_len);
#endif
}


TEST(IOBUFFER,IOBUFFER_WRIE )
{

    #ifdef _2
    mtrpc::DEFAULT_BUFFER_SIZE =8;
    mtrpc::MAX_BUFFER_PIECES = 3;
    IOBuffer b;


    struct iovec vec[3];
    int iov_num=3;
    memset(vec,0,sizeof(vec));

    b.que[0]->size = 6;
    b.que[1]->size = 7;
    b.que[2]->size = 2;

    //b.MoveRecivePtr(6);
    memset(vec,0,sizeof(vec));
    b.GetSendBuffer(vec, iov_num);

    TRACE("b size:"<<b.queNum<<",rit:"<<toString(b.begin())<<",wit"<<toString(b.end()));
    TRACE("iov_num:"<<iov_num<<"|0:"<<vec[0].iov_base<<","<<vec[0].iov_len<<"|1:"<<vec[1].iov_base<<","<<vec[1].iov_len<<"|2:"<<vec[2].iov_base<<","<<vec[2].iov_len);

    b.MoveRecivePtr(3);
    memset(vec,0,sizeof(vec));
    b.GetSendBuffer(vec, iov_num);

    TRACE("b size:"<<b.queNum<<",rit:"<<toString(b.begin())<<",wit"<<toString(b.end()));
    TRACE("iov_num:"<<iov_num<<"|0:"<<vec[0].iov_base<<","<<vec[0].iov_len<<"|1:"<<vec[1].iov_base<<","<<vec[1].iov_len<<"|2:"<<vec[2].iov_base<<","<<vec[2].iov_len);


    b.MoveRecivePtr(4);
    memset(vec,0,sizeof(vec));
    b.GetSendBuffer(vec, iov_num);

    TRACE("b size:"<<b.queNum<<",rit:"<<toString(b.begin())<<",wit"<<toString(b.end()));
    TRACE("iov_num:"<<iov_num<<"|0:"<<vec[0].iov_base<<","<<vec[0].iov_len<<"|1:"<<vec[1].iov_base<<","<<vec[1].iov_len<<"|2:"<<vec[2].iov_base<<","<<vec[2].iov_len);



    b.MoveRecivePtr(6);
    memset(vec,0,sizeof(vec));
    b.GetSendBuffer(vec, iov_num);

    TRACE("b size:"<<b.queNum<<",rit:"<<toString(b.begin())<<",wit"<<toString(b.end()));
    TRACE("iov_num:"<<iov_num<<"|0:"<<vec[0].iov_base<<","<<vec[0].iov_len<<"|1:"<<vec[1].iov_base<<","<<vec[1].iov_len<<"|2:"<<vec[2].iov_base<<","<<vec[2].iov_len);

    b.MoveRecivePtr(1);
    memset(vec,0,sizeof(vec));
    b.GetSendBuffer(vec, iov_num);

    TRACE("b size:"<<b.queNum<<",rit:"<<toString(b.begin())<<",wit"<<toString(b.end()));
    TRACE("iov_num:"<<iov_num<<"|0:"<<vec[0].iov_base<<","<<vec[0].iov_len<<"|1:"<<vec[1].iov_base<<","<<vec[1].iov_len<<"|2:"<<vec[2].iov_base<<","<<vec[2].iov_len);


    b.MoveSendPtr(5);
    memset(vec,0,sizeof(vec));
    b.GetSendBuffer(vec, iov_num);

    TRACE("b size:"<<b.queNum<<",rit:"<<toString(b.begin())<<",wit"<<toString(b.end()));
    TRACE("iov_num:"<<iov_num<<"|0:"<<vec[0].iov_base<<","<<vec[0].iov_len<<"|1:"<<vec[1].iov_base<<","<<vec[1].iov_len<<"|2:"<<vec[2].iov_base<<","<<vec[2].iov_len);
#endif
}


TEST(IOBUFFER,IOBUFFER_ITERATOR )
{
//#define _3
#ifdef _3
    mtrpc::DEFAULT_BUFFER_SIZE =8;
    mtrpc::MAX_BUFFER_PIECES = 3;
    IOBuffer b;

    b.que[0]->size = snprintf(b.que[0]->buffer,b.que[0]->size,"abc");
    b.que[1]->size = snprintf(b.que[1]->buffer,b.que[1]->size,"i");
    b.que[2]->size = snprintf(b.que[2]->buffer,b.que[2]->size,"eert");
    b.MoveRecivePtr(b.que[0]->size + b.que[1]->size +b.que[2]->size-1);
    TRACE("b size:"<<b.queNum<<",rit:"<<toString(b.begin())<<",wit:"<<toString(b.end()));

    for(IOBuffer::Iterator it = b.begin(); it != b.end(); ++it)
        printf("%d:%c\n",(b.end() - it),*it);
#endif

}



TEST(READBUF,READBUF)
{
//#define _4
#ifdef _4
    mtrpc::builtin::MethodStat m ;
    m.set_method_name("hehe");
    m.set_failed_avg_time_us(1.0);


    WriteBuffer b;
    b.BeginPacket();


    bool ret = m.SerializeToZeroCopyStream(&b);

   int writesize = b.ByteCount();

   std::string s = m.SerializeAsString();


   TRACE("b size:"<<b.queNum<<",rit:"<<toString(b.begin())<<",wit:"<<toString(b.end())<<",ret:"<<ret<<",writesize:"<<writesize<<",s zie:"<<s.size());

    mtrpc::builtin::MethodStat*  c = m.New() ;
    ReadBuffer*  readbuf  =new ReadBuffer(b);

    TRACE("readbuf size:"<<readbuf->queNum<<",rit:"<<toString(readbuf->begin())<<",wit:"<<toString(readbuf->end())<<",ret:"<<ret);


    ret = c->ParseFromZeroCopyStream(readbuf);
    TRACE("readbuf size:"<<readbuf->queNum<<",rit:"<<toString(readbuf->begin())<<",wit:"<<toString(readbuf->end())<<",ret:"<<ret);

    TRACE(c->method_name()<<","<<c->failed_avg_time_us());

#endif
}


TEST(WRITE,WRITE)
{



}



int main(int argc,char*argv[]){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

