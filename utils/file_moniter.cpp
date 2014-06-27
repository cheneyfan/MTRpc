// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: <guofutan@tencent.com>
// Created: 2013-07-01
// Description:

#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <limits.h>
#include <sys/eventfd.h>


#include "public/utils/log.h"
#include "file_moniter.h"


FileHelper::FileHelper(std::string name, boost::function<int(std::string )> readfun):
    _name(name),
    last_update_s(-1),
    _reloadfun(readfun)

{
}

bool FileHelper::isExists()
{
    return _name.size() > 0 && 0 == access(_name.c_str(),R_OK);
}

bool FileHelper::isChanged()
{
    if(last_update_s == -1)
        return false;

    return last_update_s != getUpdateTime();
}

int64_t FileHelper::getUpdateTime()
{
    struct stat sb;

    if(_name.size() == 0)
        return -1;

    if( 0 != stat(_name.c_str(), &sb))
    {
        return -1;
    }

    return sb.st_mtim.tv_sec;
}

void FileHelper::reload(){

    if(!_reloadfun)
    {
        WARN("reload function is null,file:"<<_name);
        return;
    }

    if(_reloadfun(_name) != 0)
    {
        WARN("reload "<<_name<<" failed, last change:"<<last_update_s<<",now:"<<getUpdateTime());
        return;
    }

    TRACE("reload "<<_name<<" ok, last change:"<<last_update_s<<",now:"<<getUpdateTime());

    last_update_s = getUpdateTime();
}




///
/// \brief FileMoniter
///
FileMoniter::FileMoniter():
    isrunning(true),
    notify_fd(inotify_init()),
    event_fd(eventfd(0, EFD_CLOEXEC|EFD_NONBLOCK)),
    _thread(boost::bind(&FileMoniter::check_thread, this))
{

}

FileMoniter::~FileMoniter(){

    std::map<int, FileHelper>::iterator it = moniterfiles.begin();

    for(; it!= moniterfiles.end(); ++it)
    {
        inotify_rm_watch(notify_fd, it->first);
    }

    close(event_fd);
}

///
/// \brief registerFile
/// \param fullpath
/// \param fun
/// \return
///
int FileMoniter::registerFile(std::string fullpath,
                              boost::function<int(std::string filename)> fun)
{

    if(notify_fd <= 0)
    {
        WARN("inotify create failed:"<<notify_fd<<",errno:"<<errno);
        return -1;
    }

    FileHelper filehelper(fullpath,fun);

    if(fullpath.size() == 0
            && !filehelper.isExists())
    {
        WARN("file not exists:"<<fullpath);
        return -1;
    }

    // only monitor write
    int wd = inotify_add_watch(notify_fd, fullpath.c_str(), IN_CLOSE_WRITE );

    if(wd <= 0)
    {
        WARN("inotify_add_watch failed, file:"<<fullpath<<",wd:"<<wd<<",errno:"<<errno);
        return -1;
    }

    TRACE("add file moniter:"<<filehelper._name<<",last changed:"<<filehelper.last_update_s);

    moniterfiles.insert(std::make_pair(wd, filehelper));

    return 0;
}


///
/// \brief check_files
///
void FileMoniter::check_thread()
{

    struct pollfd pfd[2] = {{ notify_fd, POLLIN, 0 },{event_fd,POLLIN, 0}};

    char buffer[BUF_LEN] = {0};
    int ret = -1;

    while(isrunning)
    {

        ret = poll(pfd, 2, CHECK_INERTERNAL_MS);

        if(ret == -1)
        {
            WARN("poll failed, ret:"<<ret<<", errno:"<<errno);
            continue;
        }

        // time out
        if(ret == 0)
        {
            continue;
        }

        if( (pfd[0].revents & POLLIN) == 0)
        {
            TRACE("wake up by event");
            break;
        }

        int nbytes = read(notify_fd, buffer, BUF_LEN);

        if(nbytes <= 0)
        {
            WARN("read fail, ret:"<<ret<<", errno:"<<errno);
            continue;
        }

        process_file_event(buffer, nbytes);

    }

}


void FileMoniter::process_file_event(char * buffer, int nbytes)
{

    struct inotify_event * begin = (struct inotify_event *)buffer;
    struct inotify_event * end   = (struct inotify_event *)(buffer + nbytes);

    while(begin < end)
    {
        TRACE("inotify_event,fd:"<<begin->wd<<",name:"<<begin->name<<",len:"<<begin->len<<",mask:"<<begin->mask<<",cookie:"<<begin->cookie);

        std::map<int, FileHelper>::iterator it = moniterfiles.find(begin->wd);

        if(it == moniterfiles.end())
        {
            WARN("find a unkown fd:"<<begin->wd<<", name:"<<begin->name);
            continue;
        }

        FileHelper & fh = it->second;
        fh.reload();
       
        // if file moniter removed,register it again
        if(begin->mask & IN_IGNORED )
        {
            FileHelper  fh = it->second;

           int wd = inotify_add_watch(notify_fd, fh._name.c_str(), IN_CLOSE_WRITE );
           if(wd <= 0)
           {
               WARN("inotify_add_watch failed, file:"<<fh._name<<",wd:"<<wd<<",errno:"<<errno);

           }else{           
               TRACE("file change register it:"<<fh._name<<",with wid:"<<wd);
               moniterfiles.erase(it);
               moniterfiles.insert(std::make_pair(wd,fh));
           }
        }

        // update to next
        char * next = ((char*)begin) + sizeof(inotify_event) + begin->len;
        begin = (struct inotify_event *)(next);
    }
}

void FileMoniter::stop(){

    isrunning = false;

    //send signal
    uint64_t stop_int = 1;
    int ret =::write(event_fd, &stop_int, sizeof(uint64_t));
    INFO("stop file moniter ret:"<<ret);
}

void FileMoniter::join(){
    _thread.join();
}


