// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: <guofutan@tencent.com>
// Created: 2013-07-01
// Description:

#ifndef _configure_h_
#define _configure_h_

#include "public/libjson/json/json.h"
#include "public/utils/log.h"
#include <iostream>
#include <fstream>
#include <string>

///
/// \brief The Configure class
///
class Configure : public Json::Value
{
public:

    Configure(const std::string& filename)
        :isload(load(filename)){

    }

    Configure(const char file[] )
        :isload(load(std::string(file)))
    {

    }

    ///
    /// \brief load
    /// \param filename
    /// \return 0 load ok
    ///         -1 load error
    ///
    int load(const std::string& filename){

        std::ifstream inf;
        inf.open(filename.c_str());

        if(!inf.is_open()){
            WARN("File Not Exist :"<<filename);
            return -1;
        }

        Json::Reader reader;
        if(! reader.parse(inf,*this)){
            WARN("File is bad Format :"<<filename);
            return -1;
        };

        Json::FastWriter writer;
        INFO("Configure:"<<filename<<",load finish,config:"<<writer.write(*this));
        return 0;
    }

public:
    int isload;

};

#endif
