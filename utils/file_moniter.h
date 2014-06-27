// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: <guofutan@tencent.com>
// Created: 2013-07-01
// Description:

#ifndef _FILE_MONITER_H_
#define _FILE_MONITER_H_


#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>


#include <string>
#include <map>




#define CHECK_INERTERNAL_MS 60*1000

#define MAX_EVENTS  8
#define LEN_NAME    256
#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( MAX_EVENTS * ( EVENT_SIZE + LEN_NAME ))

///
/// \brief The FileHelper class
///
class FileHelper {

public:

    FileHelper(){}

    ///
    /// \brief FileHelper
    /// \param name
    /// \param readfun
    ///
    FileHelper(std::string name, boost::function<int(std::string )> readfun);

    ///
    /// \brief isExists
    /// \return
    ///
    bool isExists();

    ///
    /// \brief isChanged
    /// \return
    ///
    bool isChanged();

    ///
    /// \brief getUpdateTime
    /// \return
    ///
    int64_t getUpdateTime();

    ///
    /// \brief getFileSize
    /// \return
    ///
    int32_t getFileSize()
    {

       struct stat statbuff;
       if(stat(_name.c_str(), &statbuff) < 0){
           return -1;
       }

       return statbuff.st_size;
    }

    ///
    /// \brief reload
    ///
    void reload();

public:
    std::string _name;
    int64_t last_update_s;
    boost::function<int(std::string )> _reloadfun;
};


///
/// \brief The FileMoniter class
///
class FileMoniter{

public:

    ///
    /// \brief FileMoniter
    ///
    FileMoniter();
    ~FileMoniter();

    ///
    /// \brief registerFile
    /// \param fullpath
    /// \param fun
    /// \return
    ///
    int registerFile(std::string fullpath,
                     boost::function<int(std::string filename)> fun);

    ///
    /// \brief check_files
    ///
    void check_thread();

    ///
    /// \brief process_file_event
    /// \param buffer
    /// \param size
    ///
    void process_file_event(char * buffer, int size);

    ///
    /// \brief stop
    ///
    void stop();

    ///
    /// \brief join
    ///
    void join();
public:
    volatile bool isrunning;
    int notify_fd;
    int event_fd;

    boost::thread _thread;

    std::map<int, FileHelper> moniterfiles;
};

#endif
