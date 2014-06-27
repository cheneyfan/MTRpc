// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: <guofutan@tencent.com>
// Created: 2013-07-01
// Description:

#ifndef __MODULES_H_
#define __MODULES_H_


#include "boost/shared_ptr.hpp"
#include "boost/function.hpp"
#include "boost/bind.hpp"
#include "boost/shared_array.hpp"

#include "public/libjson/json/json.h"
#include "public/jce/pdu_header.h"


#include "public/utils/log.h"
#include "public/jce/fa_proto.h"


class ModuleInterface {
public:

    ModuleInterface(){}
    virtual ~ModuleInterface(){}

    ///
    /// \brief init
    /// \param conf
    /// \return
    ///
    virtual int init(Json::Value conf) = 0;
};


///
/// \brief The ModuleGroupInterface class
///
class ModuleGroupInterface {
public:
    ModuleGroupInterface(){}
    virtual ~ModuleGroupInterface(){}

    virtual ModuleGroupInterface * create() = 0;

    ///
    /// \brief setName
    /// \param name
    ///
    virtual void setName(std::string name) = 0;

    ///
    /// \brief getName
    /// \return
    ///
    virtual std::string getName() = 0;

    ///
    /// \brief init
    /// \param config
    /// \return
    ///
    virtual int init(Json::Value config) = 0;

    ///
    /// \brief getCmdName
    /// \param cmd
    /// \return
    ///
    virtual std::string getCmdName(int cmd) = 0;

    ///
    /// \brief getCmdno
    /// \param name
    /// \return
    ///
    virtual int getCmdno(std::string name) = 0;

    ///
    /// \brief addModule
    /// \return
    ///
    virtual int addModule(ModuleInterface *) = 0;

    ///
    /// \brief proc
    /// \param reqbuffer
    /// \param reqsize
    /// \param resbuffer
    /// \param resize
    /// \return
    ///
    virtual int proc(pdu_protocol_header& header,
                     boost::shared_array<unsigned char> reqbuffer,int reqsize,
                     boost::shared_array<unsigned char> resbuffer,int ressize) = 0;

};

///
///
///
template<class Req,class Res,class Session>
class SingleModule : public ModuleInterface {

    typedef boost::function<int(pdu_protocol_header&, Req&, Res&, Session&)>  ProcFuncType;
    typedef boost::function<int(Json::Value& config)>  InitFuncType;

public:
    SingleModule(std::string name, ProcFuncType procfun,InitFuncType initfun):
        _name(name),
        _procfun(procfun),
        _initfun(initfun),
        isinited(false)
    {
    }

    ///
    /// \brief init
    /// \param conf
    /// \return
    ///
    virtual int init(Json::Value conf){

        if(isinited)
            return 0;

        if(_initfun(conf) != 0)
            return -1;

        isinited = true;
        return 0;
    }

    ///
    /// \brief proc
    /// \param header
    /// \param req
    /// \param res
    /// \param session
    /// \return
    ///
    virtual int proc(pdu_protocol_header& header,Req& req,Res& res,Session& session){
        return _procfun(header,req,res,session);
    }

public:
    std::string _name;
    ProcFuncType _procfun;
    InitFuncType _initfun;
    bool isinited;
};

template<class Cmd,class Req,class Res,class Session>
class MouduleGroup : public ModuleGroupInterface
{
    typedef SingleModule<Req,Res,Session> ModuleType;
public:



    ///
    /// \brief create
    /// \return
    ///
    virtual ModuleGroupInterface * create(){
        ModuleGroupInterface * group = new MouduleGroup<Cmd,Req,Res,Session>();
        group ->setName(_name);
        return group;
    }

    ///
    /// \brief setName
    /// \param name
    ///
    virtual void setName(std::string name){
        this->_name = name;
    }

    ///
    /// \brief getName
    /// \return
    ///
    virtual std::string getName(){
        return _name;
    }


    virtual int init(Json::Value config){
        this->_sessionconfig = config;
        return 0;
    }

    ///
    /// \brief getCmdName
    /// \param cmd
    /// \return
    ///
    virtual std::string getCmdName(int cmd){
        return face_annotation::etos((Cmd)cmd);
    }

    ///
    /// \brief getCmdno
    /// \param name
    /// \return
    ///
    virtual int getCmdno(std::string name){
        Cmd cmdno ;
        return face_annotation::stoe(name,cmdno) == 0 ? int(cmdno) : -1;
    }

    ///
    /// \brief addModule
    /// \return
    ///
    virtual int addModule(ModuleInterface* mod){
        _module_vec.push_back((ModuleType*)mod);
        INFO("module group:"<<_name<<" add :"<<((ModuleType*)mod)->_name);
        return 0;
    }


    ///
    /// \brief proc
    /// \param header
    /// \param reqbuffer
    /// \param reqsize
    /// \param resbuffer
    /// \param ressize
    /// \return
    ///
    virtual int proc(pdu_protocol_header& header,
                     boost::shared_array<unsigned char> reqbuffer,int reqsize,
                     boost::shared_array<unsigned char> resbuffer,int ressize){

        //unpack
        Req req;
        Res res;
        pdu_protocol_header reqheader = header;

        //unpack the request
        int hr = 0;
        if((hr = unpack_request(reqbuffer.get(),reqsize,&req, &header)) !=0 )
        {
            ERROR("unpack request error,cmd:"<<getCmdName(reqheader.cmd)<<",request len:"<<reqheader.len<< "ret " << hr);
            return -1;
        }

        // every thread has a session is store in TLS
        if(ThreadLocalSingleton<Session>::pointer() == NULL)
        {
            int ret = ThreadLocalSingleton<Session>::instance().init(_sessionconfig);
            if(ret!=0)
            {
                ERROR("init session,ret"<<ret);
            }
        }

        std::stringstream logstream;


        Session& session = ThreadLocalSingleton<Session>::instance();

        session.begin(reqheader);
        uint64_t alltimems = 0;
        int ret = 0;
        for(unsigned int i = 0 ; i < _module_vec.size(); ++i)
        {
            uint64_t timems = TimerHelper::Now_Millisecond();

            ret = _module_vec[i]->proc(reqheader,req,res,session);

            timems = TimerHelper::Now_Millisecond() - timems;
            alltimems+=timems;

            logstream<<_module_vec[i]->_name<<":"<<timems<<", ";

            if(ret != 0){
               break;
            }
        }
        logstream<<", alltime:"<<alltimems;

        logstream<<", request:";
        req.display(logstream);

        ThreadLocalSingleton<Session>::instance().reset();

        //copy header
        if(pack_response(resbuffer.get(),ressize,&res,&header) != 0)
        {
             WARN("pack response error,"<<"header:"<<header<<logstream.str()<<", response:"<<res<<",send header only!");

             return -1;
        }

        if(ret == 0) {
            INFO("Proc Finish,reqheader:"<<reqheader<<logstream.str()<<", response:"<<res<<",resheader:"<<header);
        } else {
            WARN("Proc Fail,reqheader:"<<reqheader<<logstream.str()<<", response:"<<res<<",resheader:"<<header<<",ret:"<<ret);
        }

        return ret;
    }


    ///
    /// \brief unpack_request need  template specialization
    /// \return
    ///
    int unpack_request(unsigned char * buffer, int size, Req* req, pdu_protocol_header * header){
        return req->Decode((uint8_t*)buffer,&size,header);
    }
    ///
    /// \brief pack_response need  template specialization
    /// \return
    ///
    int pack_response(unsigned char * buffer, int size, Res* res,pdu_protocol_header * header){

        int ret = res->Encode((uint8_t*)buffer,&size,header);

        //copy headers
        pdu_protocol_header* resheader = (pdu_protocol_header* )(buffer+1);

        resheader->response_flag = header->response_flag;
        resheader->response_info = htons(header->response_info);

        resheader->key           = htonl(header->key);
        resheader->seq           = htonl(header->seq);

        //set pack len
        header->len = size;

        return ret;
    }

public:
    std::vector<ModuleType*> _module_vec;
    Json::Value _sessionconfig;
    std::string _name;

};

////
/// \brief The ModuleManger class
///
template<class Session>
class ModuleManger {

public:

    ~ModuleManger(){
        //delete
        std::map<std::string, ModuleInterface* >::iterator it
                = _moduleprocs.begin();

        for( ; it != _moduleprocs.end(); ++it)
        {
            INFO("delete module :"<<it->first);
            delete it->second;
        }

        //delete
        for(unsigned int i = 0; i< _modulegroupvec.size();++i)
        {
            INFO("delete module group info: "<<_modulegroupvec[i]->getName());
            delete _modulegroupvec[i];
        }

        //delete

        std::map<int,ModuleGroupInterface*>::iterator itg = _modulegroupmap.begin();
        for(; itg!=_modulegroupmap.end();++itg)
        {
            INFO("delete module group cmd:"<<itg->first<<",name:"<<itg->second->getName());
            delete itg->second;
        }


    }


    int init(Json::Value config){

        // init modules;
        Json::Value module_config = config["modules"];
        Json::Value _sessionconfig = config["session"];

        std::vector<std::string> module_cmds = module_config.getMemberNames();

        if(module_config.isNull() || module_cmds.size() == 0)
        {
            WARN("module config no modules founded");
            return -1;
        }

        std::vector<std::string>::iterator it = module_cmds.begin();
        for(; it!= module_cmds.end(); ++it)
        {
             Json::Value module_names = module_config[*it];

             int cmdno = -1;
             ModuleGroupInterface * group = NULL;

             for(unsigned int i = 0; i < _modulegroupvec.size(); ++i)
             {
                 cmdno = _modulegroupvec[i]->getCmdno(*it);

                 if( cmdno >= 0  ){
                     group = _modulegroupvec[i];
                     break;
                 }
             }

             if(cmdno == -1 || group == NULL){
                 WARN("unkonw cmdname:"<<*it);
                 return -1;
             }

             if(_modulegroupmap.find(cmdno) != _modulegroupmap.end())
             {
                 WARN("already configed,cmdno:"<<cmdno);
                 return -1;
             }

             // every has a new group
             _modulegroupmap[cmdno] = group->create();
             std::string groupname = group->getCmdName(cmdno)+":";
             groupname += group->getName();
             _modulegroupmap[cmdno]->setName(groupname);

             _modulegroupmap[cmdno]->init(_sessionconfig);

             std::stringstream logstream;
             logstream<<" Cmd:"<<*it<<", cmdnno:"<<cmdno<<", ";

             for(unsigned int i =0;i < module_names.size(); ++i)
             {
                 std::string proc_name = module_names[i].asString();

                 if(proc_name.length() == 0
                         || _moduleprocs.find(proc_name) == _moduleprocs.end())
                 {
                     WARN("module_cmds:"<<*it<<",cmdno:"<<(int) cmdno<<",proc:"<<proc_name<<",not register");
                     return -1;
                 }

                 ModuleInterface * mod = _moduleprocs[proc_name];

                 int ret = mod->init(config[proc_name]);

                 if(ret !=0 )
                 {
                     WARN("module_cmds:"<<*it<<",cmdno:"<<(int) cmdno<<",proc:"<<proc_name<<",init fail ret:"<<ret);
                     return -1;
                 }

                 if(i != 0 )
                     logstream <<" -> ";

                 logstream<<proc_name;
                 _modulegroupmap[cmdno]->addModule(mod);
             }
             INFO("init modules:"<<logstream.str()<<" done");
        }

        return 0;
    }

    template<class Cmd, class Req,class Res,class Proc>
    int registerProc(std::string name,boost::shared_ptr<Proc> p){

        if(_moduleprocs.find(name) != _moduleprocs.end())
            return -1;

        boost::function<int(pdu_protocol_header&,Req&,Res&,Session&)> procfun =
                boost::bind(&Proc::proc, p, _1, _2, _3, _4);
        boost::function<int(Json::Value&)> initfun =
                boost::bind(&Proc::init, p,_1);

        //
        ModuleInterface * mod= new SingleModule<Req,Res,Session>(name, procfun, initfun);

        _moduleprocs[name] = mod;


        INFO("register module :"<<name);

        std::string modulelistname = Req::className() + ",";
        modulelistname = modulelistname + Res::className();

        if(_modulegroupnames.find(modulelistname) == _modulegroupnames.end())
        {
            _modulegroupnames.insert(modulelistname);

            ModuleGroupInterface* group = new
                    MouduleGroup<Cmd,Req,Res,Session>();

            group->setName(modulelistname);

            INFO("register module group:"<<modulelistname);

            _modulegroupvec.push_back(group);
        }

        return 0;
    }


    int proc(pdu_protocol_header& header,
             boost::shared_array<unsigned char>& reqbuffer,int reqsize,
             boost::shared_array<unsigned char>& resbuffer,int ressize){

        if(_modulegroupmap.find(header.cmd) == _modulegroupmap.end())
		{
			WARN("_modulegroupmap.find(header.cmd) == _modulegroupmap.end()");
            return -1;
		}
        return _modulegroupmap[header.cmd]->proc(header,reqbuffer,reqsize,resbuffer,ressize);
    }

    ///
    /// \brief vaildCmd
    /// \param cmdno
    /// \return
    ///
    bool vaildCmd(int cmdno){
        return _modulegroupmap.find(cmdno) == _modulegroupmap.end() ? false : true;
    }

    std::string getCmdname(int cmdno){
        std::map<int, ModuleGroupInterface*>::iterator it = _modulegroupmap.find(cmdno);

        if(it == _modulegroupmap.end())
            return std::string("");

        return it->second->getCmdName(cmdno);
    }

public:

    std::map<int, ModuleGroupInterface*> _modulegroupmap;
    std::map<std::string, ModuleInterface* > _moduleprocs;

    std::vector<ModuleGroupInterface*> _modulegroupvec;
    std::set<std::string> _modulegroupnames;
};
#endif
