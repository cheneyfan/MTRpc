#ifndef _configure_h_
#define _configure_h_

#include "json/json/json.h"
#include <iostream>
#include <fstream>
#include <string>

namespace mtrpc {


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
            std::cout<<"File Not Exist :"<<std::endl;
            return -1;
        }

        Json::Reader reader;
        if(! reader.parse(inf,*this)){
            std::cout<<"File is bad Format :"<<std::endl;
            return -1;
        };

        Json::FastWriter writer;
        std::cout<<"Configure:"<<filename<<",load finish"<<std::endl;
        return 0;
    }

public:
    int isload;

};


}

#endif
