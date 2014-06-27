#ifndef __NEW_MULTISERVER_H_
#define __NEW_MULTISERVER_H_

#include <sys/types.h>
#include <ifaddrs.h>
#include <sstream>
#include <iostream>

#include "boost/shared_ptr.hpp"
#include "boost/function.hpp"
#include "boost/bind.hpp"
#include "boost/shared_array.hpp"

#include "public/libjson/json/json.h"
#include "public/jce/pdu_header.h"
#include "public/utils/log.h"
#include "public/jce/fa_proto.h"
#include "public/jce/file_upload.h"
#include "public/jce/QuanPhotoUpload.h"
#include "public/jce/wup.h"

#include "public/utils/singleton.h"
#include "public/utils/serverstat.h"
#include "modules.h"
#include "public/asio/ieventhandler.h"
#include "public/asio/server.h"
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

using namespace FileUpload;
using namespace QuanPhotoUpload;
using namespace face_annotation;

namespace youtu_asio
{

void facerect_to_annorect(const QuanPhotoUpload::FaceRect& face_rect, face_annotation::rect& anno_rect);
void annorect_to_facerect(const face_annotation::rect& anno_rect, QuanPhotoUpload::FaceRect& face_rect);
void facepoint_to_annorect(const QuanPhotoUpload::FacePoint& face_point, face_annotation::point& anno_point);
void annopoint_to_facerect(const face_annotation::point& anno_point, QuanPhotoUpload::FacePoint& face_point);
void faceitem_to_annoitem(const QuanPhotoUpload::FaceItem& face_item, const string& data, face_annotation::face_raw_item& anno_item);
void annoitem_to_faceitem(face_annotation::face_raw_item& anno_item, QuanPhotoUpload::FaceItem& face_item, string& data);
void htonl_pdu_protocol_header(pdu_protocol_header& header);
void ntohl_pdu_protocol_header(pdu_protocol_header& header);

template<class Session>
class MultiServer : public IIOEventHandler
{
public:
    typedef MultiServer<Session> this_type;
    template<class Cmd, class Req,class Res,class Proc>
    int registerProc(std::string name, Proc* p)
    {
        using namespace std;
        return mmanger.registerProc<Cmd,Req,Res,Proc>(name, boost::shared_ptr<Proc>(p));
    }

    template<class Cmd, class Req,class Res,class Proc,int N>
    int registerProc(const char name[N],Proc *p)
    {
        using namespace std;
        return mmanger.registerProc<Cmd,Req,Res,Proc>(std::string(name), boost::shared_ptr<Proc>(p));
    }

    int init(const Json::Value& config);
    int stop();
    int run();
    int start();
    int join();
    std::string getAvailableIp();
    int proc(connection_ptr sp, boost::shared_array<unsigned char> req_buff, int size);

	int rebuildReq(boost::shared_array<unsigned char>& req_buff, int32_t& req_buff_size, pdu_protocol_header& pdu_header, SvcRequestHead& svc_header);
	int rebuildRsp(boost::shared_array<unsigned char>& rsp_buff, int32_t& rsp_buff_size, const SvcRequestHead& svc_head);

public:
    virtual void onSendSuccess(connection_ptr sp)
    {
        sp->receiveHead();
    }
    virtual void onSendFailure(connection_ptr sp, const boost::system::error_code& e)
    {
        WARN("MultiServer::onSendFailure " << e.message() << " " << sp->ip() << ":" << sp->port());
    }

    virtual void onReceiveHeadSuccess(connection_ptr sp, boost::shared_array<unsigned char>& data, unsigned length)
    {
        pdu_protocol_header * pheader = (pdu_protocol_header *)(data.get() + 1);
        pdu_protocol_header hostheader = *pheader;
        net2host(&hostheader);

        std::string cmd_name = mmanger.getCmdname(hostheader.cmd);
        if(0 == cmd_name.size()) {
            WARN("unkonwn header:" << hostheader);
            return;
        }
        if( hostheader.len <= CConnection::PKG_HEAD_LENGTH || hostheader.len >= CConnection::MAX_BUFFER_LENGTH) {
            WARN("head unpack error,cmd:"<<cmd_name<<",request header:"<<hostheader<<",buffer sz" << length << ",hostheader.len" << hostheader.len);
            return;
        }
        sp->receiveBody(hostheader.len - CConnection::PKG_HEAD_LENGTH);
    }

    // data contains header and body and extra 2 bytes
    virtual void onReceiveBodySuccess(connection_ptr sp, boost::shared_array<unsigned char>& data, unsigned length)
    {
        TRACE("start proc data size " << length);
        proc(sp, data, length);
    }

    virtual void onReceiveFailure(connection_ptr sp, const boost::system::error_code& e)
    {
        if (e != boost::asio::error::eof) {
            WARN("onReceiveFailure " << e.message() << " ip " << sp->ip() << ":" << sp->port());
        }
        TRACE("MultiServer::onReceiveFailure " << e.message());
    }

    virtual void onCommonError(connection_ptr sp, unsigned ec, const std::string& em)
    {
        TRACE("MultiServer::onCommonError " << ec << " " << em);
    }

    virtual void onTimeoutFailure(connection_ptr sp, const boost::system::error_code& e)
    {
        TRACE("MultiServer::onTimeoutFailure " << e.message());
    }

private:
    std::set<uint32_t> m_rebuild_cmdmap;
    Json::Value m_config;
    std::string m_ip;
    unsigned m_port;
    unsigned thread_num;
    ModuleManger<Session> mmanger;
    boost::shared_ptr<CServer> m_spserver;
    boost::shared_ptr<boost::thread> m_sploop;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Session>
int MultiServer<Session>::join()
{
    m_sploop->join();
    return 0;
}

template<class Session>
int MultiServer<Session>::run()
{
    m_spserver->run();
    return 0;
}

template<class Session>
int MultiServer<Session>::stop()
{
    m_spserver->stop();
    return 0;
}

template<class Session>
int MultiServer<Session>::start()
{
    if (m_spserver == NULL) {
        ERROR("youtu_asio::CServer pointer is null!");
        return 1;
    }
    if(m_sploop.get() == NULL){
        boost::function<void (void)> runfunction = boost::bind(&this_type::run, this);
        m_sploop.reset(new boost::thread(runfunction));
    }
    return 0;
}

inline int GetCpuNumber()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}

template<class Session>
int MultiServer<Session>::init(const Json::Value& config)
{
    m_rebuild_cmdmap.insert(FA_OFFLINE_RECOM_ONLY_REQUEST);

    m_config = config;
    Json::Value server_config = m_config["server"];

    if(server_config.isMember("ip")){
        m_ip = server_config["ip"].asString();
    }else{
        m_ip = getAvailableIp();
    }

    if(m_ip.size() == 0)
        m_ip = "127.0.0.1";

    if(inet_addr(m_ip.c_str())==0){
        WARN("Bad ip:" << m_ip);
        return -1;
    }

    m_port = server_config["port"].asInt();
    if(m_port <= 1024 || m_port == 8080 || m_port == 36000) {
        WARN("Please Don't use high risk port !");
        return -1;
    }

    thread_num = GetCpuNumber() * 4;

    INFO("thread_num is set to " << thread_num << " based on cpu num of this machine");
    if (thread_num <= 0) {
        thread_num = server_config["thread_num"].asInt();
        WARN("something wrong with GetCpuNumber api. Use thread_num " << thread_num << " read from conf instead");
    }
    
    if(0 != mmanger.init(config)) {
        using namespace std;
        Json::FastWriter writer;
        WARN("init modules fail" << writer.write(config));
        return -1;
    }

    Session s;

    int ret = s.init(config["session"]);
    if(ret != 0)
    {
        Json::FastWriter writer;
        WARN("init session fail,ret:"<<ret<<",config:"<< writer.write(config["session"]));
        return ret;
    }

    stringstream ss;
    ss << m_port;
    m_spserver.reset(new CServer(thread_num, this));
    ret = m_spserver->listen(m_ip, ss.str());
    if (ret == 0) {
        INFO("create server success! listen " << m_ip << ":" << ss.str());
    } else {
        ERROR("fail to create server! try to listen " << m_ip << ":" << ss.str());
    }
    return ret;
}


// for cmd102, convert buff to their structs, then to our structs, then to buff finally.
template<class Session>
int MultiServer<Session>::rebuildReq(boost::shared_array<unsigned char>& req_buff, int32_t& req_buff_size, pdu_protocol_header& pdu_header, SvcRequestHead& svc_head)
{
    int ret = 0;

    // 1. extract SvcRequestHead
    unsigned char* pSvcHead = req_buff.get() + 1 + sizeof(pdu_protocol_header) + 1;
    int svc_head_len = ntohl(*(int*)pSvcHead);
	if(svc_head_len < 4)
	{
		WARN("svc_head_len < 4.");
		return -1;
	}
    string strSvcHead;
    strSvcHead.assign((char*)pSvcHead, svc_head_len);
    try
    {
        wup::UniPacket<> server;
        server.decode(strSvcHead.c_str(), svc_head_len);
        server.get<SvcRequestHead>("SvcRequestHead", svc_head);
    }
    catch (std::runtime_error& e)
    {
		WARN("wup decode fail.");
    	return -1;
    }
    catch(...)
    {
		WARN("wup decode fail.");
        return -1;
    }

    // 2. extract QuanPhotoReq
    unsigned char* pDesc = pSvcHead + svc_head_len;
    int desc_len = svc_head.iDescLen;
	int desc_len2 = ntohl(*(int*)pDesc);
	if(desc_len < 4 || desc_len != desc_len2)
	{
		WARN("desc_len < 4 || desc_len != desc_len2.");
		return -1;
	}
    string strDesc;
    strDesc.assign((char*)pDesc, desc_len);
    QuanPhotoReq photo_req;
	try
	{
		wup::UniPacket<> server;
		server.decode(strDesc.c_str(), desc_len);
		server.get<QuanPhotoReq>("QuanPhotoReq", photo_req);
	}
	catch (std::runtime_error& e)
	{
		WARN("wup decode fail.");
		return -1;
	}
	catch(...)
	{
		WARN("wup decode fail.");
		return -1;
	}

    const vector<FaceItem>& face_vec = photo_req.faceItems;
    offline_request_pkg req_pkg;

    // 3. verify packet length
    unsigned legal_packet_len = 1 + sizeof(pdu_protocol_header) + svc_head_len + svc_head.iDescLen;
    for(unsigned m = 0; m < face_vec.size(); m++)
    {
        legal_packet_len += face_vec[m].size;
    }
    if(legal_packet_len != (unsigned)req_buff_size)
    {
        WARN("legal_packet_len: " << legal_packet_len << "   req_buff_size: " << req_buff_size);
        return -1;
    }

    // 4. extract File
    unsigned char* pFile = pDesc + desc_len;
    unsigned char* pCurFile = pFile;
    string data;
    req_pkg.face_items.reserve(face_vec.size());
    for(unsigned m = 0; m < face_vec.size(); m++)
    {
        face_raw_item item;
        data.assign((char*)pCurFile, face_vec[m].size);
        pCurFile += face_vec[m].size;
        faceitem_to_annoitem(face_vec[m], data, item);
        req_pkg.face_items.push_back(item);
    }

    // 5. convert to buff
    int res_buff_size = CConnection::MAX_BUFFER_LENGTH;
    req_buff.get()[0] = ISDProtocolSOH;
    req_buff.get()[sizeof(pdu_protocol_header)+1] = ISDProtocolEOT;

    ret = struct_offline_request_pkg_pack(&req_pkg, (uint8_t*)req_buff.get(), &res_buff_size, &pdu_header);  // will write the extra 2 bytes
    if(0 != ret)
    {
		WARN("struct_offline_request_pkg_pack fail, status: " << ret);
        return ret;
    }
	if(res_buff_size < 0)
	{
		WARN("resbuffersize < 0.");
		return -1;
	}
    pdu_header.len = res_buff_size;
    req_buff_size = res_buff_size;
	WARN("after rebuild, pdu_header.len: " << pdu_header.len);
    pdu_protocol_header htonl_pdu_header = pdu_header;
    htonl_pdu_protocol_header(htonl_pdu_header);
    memcpy(req_buff.get() + 1, (char*)&htonl_pdu_header, sizeof(pdu_protocol_header));
    //req_buff.reset(spbuf.get());

    return ret;
}

// for cmd102, convert buff to our structs, then to their structs, then to buff finally.
template<class Session>
int MultiServer<Session>::rebuildRsp(boost::shared_array<unsigned char>& rsp_buff, int32_t& rsp_buff_size, const SvcRequestHead& svc_head)
{
    int ret = 0;

    // 1. convert to our structs
    offline_response_pkg res_pkg;
    pdu_protocol_header pdu_header;
    ret = struct_offline_response_pkg_unpack((uint8_t*)rsp_buff.get(), &rsp_buff_size, &res_pkg, &pdu_header);
    if(0 != ret)
    {
		WARN("struct_offline_response_pkg_unpack fail, status: " << ret);
        return ret;
    }

    // 2. convert to QuanPhotoReq
    QuanPhotoRsp photo_rsp;
    vector<face_raw_item>& face_items = res_pkg.face_items;
    FaceItem face_item;
    string data;   // no using
    for(unsigned m = 0; m < face_items.size(); m++)
    {
        annoitem_to_faceitem(face_items[m], face_item, data);
        photo_rsp.faceItems.push_back(face_item);
    }

    string strPhotoRsp;
    {
        wup::UniPacket<> server;
        server.setRequestId(0);
        server.setServantName("SUIPAI_SERVANT_NAME");
        server.setFuncName("SUIPAI_FUNCTION_NAME");
        server.put<QuanPhotoRsp>("QuanPhotoRsp", photo_rsp);
        server.encode(strPhotoRsp);
    }
    
    // 3. calc SvcRequestHead
    SvcResponsePacket svc_rsp_packet;
    svc_rsp_packet.vRspData.reserve(strPhotoRsp.size());
    svc_rsp_packet.vRspData.assign(strPhotoRsp.begin(), strPhotoRsp.end());
    svc_rsp_packet.iRetCode = pdu_header.response_flag;
    svc_rsp_packet.iRetSubCode = pdu_header.response_flag;
    char sResultDes[20];
    sprintf(sResultDes, "%u", (unsigned int)pdu_header.response_info);
    svc_rsp_packet.sResultDes = sResultDes;
    svc_rsp_packet.iUploadType = svc_head.iUploadType;
    svc_rsp_packet.iCmdID = svc_head.iCmdID;
    svc_rsp_packet.seq = svc_head.seq;
    svc_rsp_packet.iUin = svc_head.iUin;
    svc_rsp_packet.sFileMD5 = svc_head.sFileMD5;
    svc_rsp_packet.sDescMD5 = svc_head.sDescMD5;

    string strSvcRspPacket;
    {
        wup::UniPacket<> server;
        server.setRequestId(0);
        server.setServantName("SUIPAI_SERVANT_NAME");
        server.setFuncName("SUIPAI_FUNCTION_NAME");
        server.put<SvcResponsePacket>("SvcResponsePacket", svc_rsp_packet);
        server.encode(strSvcRspPacket);
    }

    // 4. convert to buff
    rsp_buff_size = 1 + sizeof(pdu_protocol_header) + 1 + strSvcRspPacket.size();
    pdu_header.len = rsp_buff_size;
    rsp_buff.get()[0] = ISDProtocolSOH;
    rsp_buff.get()[sizeof(pdu_protocol_header)+1] = ISDProtocolEOT;
    pdu_protocol_header htonl_pdu_header = pdu_header;
    htonl_pdu_protocol_header(htonl_pdu_header);
    memcpy(rsp_buff.get() + 1, (char*)&htonl_pdu_header, sizeof(pdu_protocol_header));
    memcpy(rsp_buff.get() + 1 + sizeof(pdu_protocol_header) + 1, strSvcRspPacket.c_str(), strSvcRspPacket.size());
    //rsp_buff.reset(spbuf.get());

    return ret;
}

template<class Session>
int MultiServer<Session>::proc(connection_ptr sp, boost::shared_array<unsigned char> req_buff, int size)
{
    int ret = 0;
    int res_buff_size = CConnection::MAX_BUFFER_LENGTH;
    boost::shared_array<unsigned char> spbuf(new unsigned char[res_buff_size]);

    pdu_protocol_header* ptrheader = (pdu_protocol_header*)(req_buff.get()+1);
    pdu_protocol_header header;

    header.cmd = ntohl(ptrheader->cmd);
    header.len = ntohl(ptrheader->len);
    header.response_flag = 0;
    header.response_info = 0;
    header.key = ntohl(ptrheader->key);
    header.seq = ntohl(ptrheader->seq);

    SvcRequestHead svc_head;
    if (m_rebuild_cmdmap.find(header.cmd) != m_rebuild_cmdmap.end())   // for cmd102, modify req_buff
    {
        int32_t req_buff_size = size;
        ret = rebuildReq(req_buff, req_buff_size, header, svc_head);
		if(ret < 0)
		{
			WARN("rebuildReq fail, status: " << ret);
		}
    }
    
    if(0 == ret)
    {
        ret = mmanger.proc(header, req_buff, header.len, spbuf, res_buff_size);
    }

    // proc fail
    // if fail, send the request's pdu header, else send the response
    if(ret < 0)
    {
        spbuf.get()[0] = ISDProtocolSOH;
        spbuf.get()[sizeof(pdu_protocol_header)+1] = ISDProtocolEOT;
        header.len = htonl(sizeof(pdu_protocol_header)+2);
        header.response_flag = 0;
        if(header.response_info == 0)
            header.response_info = htons(ret);
        else
            header.response_info = htons(header.response_info);

        header.key = ntohl(ptrheader->key);
        header.seq = ntohl(ptrheader->seq);
        memcpy(spbuf.get()+1, (char*)&header, sizeof(pdu_protocol_header));

        sp->send(spbuf.get(), sizeof(pdu_protocol_header) + 2);
        WARN("proc fail, ret:" << ret << ",send header only:" << header);
    }
    else
    {
        if (m_rebuild_cmdmap.find(header.cmd) != m_rebuild_cmdmap.end())   // for cmd102, modify spbuf
        {
            int32_t rsp_buff_size = header.len;
            ret = rebuildRsp(spbuf, rsp_buff_size, svc_head);
            header.len = rsp_buff_size;
			if(ret < 0)
			{
				WARN("rebuildRsp fail, status: " << ret);
			}
        }

        WARN("sp->send length: " << header.len);
        sp->send(spbuf.get(), header.len);
    }

    return ret;
}

template<class Session>
std::string MultiServer<Session>::getAvailableIp(){
    struct ifaddrs * if_addr = NULL;
    std::string ip = "";

    if(0 != getifaddrs(&if_addr))
    {
        return ip;
    }

    struct ifaddrs * if_addr_back = if_addr;

    for(; if_addr != NULL; if_addr = if_addr->ifa_next)
    {
        // not ipv4
        if (if_addr->ifa_addr->sa_family != AF_INET)
            continue;

        void * tmp = &((struct sockaddr_in *)if_addr->ifa_addr)->sin_addr;

        char buffer[256]={0};

        inet_ntop(AF_INET, tmp, buffer, INET_ADDRSTRLEN);

        ip = buffer;

        if(ip.size() == 0
                || ip.find("127.0")== 0
                || ip.find("10.") != 0
                || ip.find("172.")!= 0)
            continue;

        break;
    }

    if(if_addr_back)
        freeifaddrs(if_addr_back);

    return ip;
}

void facerect_to_annorect(const QuanPhotoUpload::FaceRect& face_rect, face_annotation::rect& anno_rect)
{
    anno_rect.x = face_rect.x;
    anno_rect.y = face_rect.y;
    anno_rect.w = face_rect.w;
    anno_rect.h = face_rect.h;
}

void annorect_to_facerect(const face_annotation::rect& anno_rect, QuanPhotoUpload::FaceRect& face_rect)
{
    face_rect.x = anno_rect.x;
    face_rect.y = anno_rect.y;
    face_rect.w = anno_rect.w;
    face_rect.h = anno_rect.h;
}

void facepoint_to_annorect(const QuanPhotoUpload::FacePoint& face_point, face_annotation::point& anno_point)
{
    anno_point.x = face_point.x;
    anno_point.y = face_point.y;
}

void annopoint_to_facerect(const face_annotation::point& anno_point, QuanPhotoUpload::FacePoint& face_point)
{
    face_point.x = anno_point.x;
    face_point.y = anno_point.y;
}

void faceitem_to_annoitem(const QuanPhotoUpload::FaceItem& face_item, const string& data, face_annotation::face_raw_item& anno_item)
{
    anno_item.uin = face_item.uin;
    anno_item.photo_id = face_item.photoId;
    anno_item.photo_url = "photo_url";
    anno_item.data = data;
    facerect_to_annorect(face_item.faceRect, anno_item.face_rect);
    anno_item.confidence = face_item.confidence;
    anno_item.faceid = face_item.faceId;
    anno_item.property = face_item.property;
    facepoint_to_annorect(face_item.leftEye, anno_item.left_eye);
    facepoint_to_annorect(face_item.rightEye, anno_item.right_eye);
    facerect_to_annorect(face_item.regionRect, anno_item.region_rect);
    anno_item.identity = face_item.identity;
    anno_item.photo_width = face_item.photoWidth;
    anno_item.photo_height = face_item.photoHeight;
    anno_item.album_id = face_item.albumId;
}

void annoitem_to_faceitem(face_annotation::face_raw_item& anno_item, QuanPhotoUpload::FaceItem& face_item, string& data)
{
    face_item.uin= anno_item.uin;
    face_item.photoId = anno_item.photo_id;
    data = anno_item.data;
    annorect_to_facerect(anno_item.face_rect, face_item.faceRect);
    face_item.confidence = anno_item.confidence;
    face_item.faceId = anno_item.faceid;
    face_item.property = anno_item.property;
    annopoint_to_facerect(anno_item.left_eye, face_item.leftEye);
    annopoint_to_facerect(anno_item.right_eye, face_item.rightEye);
    annorect_to_facerect(anno_item.region_rect, face_item.regionRect);
    face_item.identity = anno_item.identity;
    face_item.photoWidth = anno_item.photo_width;
    face_item.photoHeight = anno_item.photo_height;
    face_item.albumId = anno_item.album_id;
    face_item.size = anno_item.data.size();
}

void htonl_pdu_protocol_header(pdu_protocol_header& header)
{
    header.cmd = htonl(header.cmd);
    header.checksum = htonl(header.checksum);
    header.seq = htonl(header.seq);
    header.key = htonl(header.key);
    header.response_info = htonl(header.response_info);
    header.len = htonl(header.len);
}

void ntohl_pdu_protocol_header(pdu_protocol_header& header)
{
    header.cmd = ntohl(header.cmd);
    header.checksum = ntohl(header.checksum);
    header.seq = ntohl(header.seq);
    header.key = ntohl(header.key);
    header.response_info = ntohl(header.response_info);
    header.len = ntohl(header.len);
}

}


#endif //__NEW_MULTISERVER_H_
