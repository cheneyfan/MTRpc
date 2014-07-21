// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author: <guofutan@tencent.com>
// Created: 2013-07-01
// Description:

#ifndef _TIME_HELPER_H_
#define _TIME_HELPER_H_

#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <stdarg.h>
#include <string>
#include <sstream>


namespace mtrpc {
/**
 * @brief The Timer class
 */

class TimerHelper
{

public:

    ///
    /// \brief Now_Microsecond
    /// \return
    ///
    static uint64_t Now_Microsecond(){
        struct timespec tp;
        clock_gettime(CLOCK_REALTIME, &tp);
        return ((uint64_t)tp.tv_sec)*1000000 + tp.tv_nsec/1000;
    }


    ///
    /// \brief Now_Millisecond
    /// \return
    ///
    static uint64_t Now_Millisecond(){
        struct timespec tp;
        clock_gettime(CLOCK_REALTIME, &tp);
        return ((uint64_t)tp.tv_sec)*1000 + tp.tv_nsec/1000000;
    }


    ///
    /// \brief get_next_second_ms
    /// \return
    ///
    static uint64_t Next_Second_Millisecond(){

        time_t now_t = time(NULL);
        struct tm now_tm;
        localtime_r(&now_t,&now_tm);

        now_tm.tm_sec  += 1;

        return mktime(&now_tm)*1000;
    }

    ///
    /// \brief get_next_minute_ms
    /// \return
    ///
    static uint64_t Next_Minute_Millisecond(){
        time_t now_t = time(NULL);
        struct tm now_tm;
        localtime_r(&now_t,&now_tm);

        now_tm.tm_min  += 1;
        now_tm.tm_sec  = 0;

        return mktime(&now_tm)*1000;
    }

    ///
    /// \brief get_next_hour_ms
    /// \return
    ///
    static uint64_t Next_Hour_Millisecond(){

        time_t now_t = time(NULL);
        struct tm now_tm;
        localtime_r(&now_t,&now_tm);

        now_tm.tm_hour += 1;
        now_tm.tm_min  = 0;
        now_tm.tm_sec  = 0;

        return mktime(&now_tm)*1000;

    }


    ///
    /// \brief get_next_day_ms
    /// \return
    ///
    static uint64_t Next_Day_Millisecond(){

        time_t now_t = time(NULL);
        struct tm now_tm;
        localtime_r(&now_t,&now_tm);

        now_tm.tm_mday += 1;
        now_tm.tm_hour = 0;
        now_tm.tm_min  = 0;
        now_tm.tm_sec  = 0;

        return mktime(&now_tm)*1000;

    }

    ///
    /// \brief get_ms_at
    /// \param day
    /// \param hour
    /// \param minute
    /// \param second
    /// \return
    ///
    static uint64_t GetMillisecond(int day, int hour, int minute, int second)
    {
        time_t now_t = time(NULL);
        struct tm now_tm;
        localtime_r(&now_t,&now_tm);

        now_tm.tm_mday = day;
        now_tm.tm_hour = hour;
        now_tm.tm_min  = minute;
        now_tm.tm_sec  = second;

        return mktime(&now_tm)*1000;
    }





    /**
     * @brief now_string
     * @return
     */
    static void Now_Millisecond_String(char *buffer,uint32_t size){


        struct timeval tv;
        struct timezone tz;
        gettimeofday(&tv, &tz);

        struct tm now_tm;
        localtime_r(&tv.tv_sec,&now_tm);
        snprintf(buffer,size,"%04d-%02d-%02d %02d:%02d:%02d.%03d",
                 now_tm.tm_year+1900,now_tm.tm_mon+1,now_tm.tm_mday,
                 now_tm.tm_hour,now_tm.tm_min,now_tm.tm_sec,int(tv.tv_usec/1000));

    }


    /**
     * @brief now_string
     * @return
     */
    static void Get_Millisecond_String(timer_t timestamp,int32_t mills,char *buffer,uint32_t size){

        struct tm now_tm;

        localtime_r((const time_t *)&timestamp,(struct tm *)&now_tm);

        snprintf(buffer,size,"%04d-%02d-%02d %02d:%02d:%02d.%03d",
                 now_tm.tm_year+1900,now_tm.tm_mon+1,now_tm.tm_mday,
                 now_tm.tm_hour,now_tm.tm_min,now_tm.tm_sec,mills);
    }

    /**
     * @brief now_string
     * @return
     */
    static void  Now_Time_String(char *buffer,uint32_t size){
        struct tm now_tm;

        time_t nows=time(NULL);
        localtime_r(&nows,&now_tm);
        snprintf(buffer,size,"%04d%02d%02d_%02d%02d%02d",
                 now_tm.tm_year+1900,now_tm.tm_mon+1,now_tm.tm_mday,
                 now_tm.tm_hour,now_tm.tm_min,now_tm.tm_sec);

    }

    /**
     * @brief now_string
     * @return
     */
    static void Now_Date_String(char *buffer,uint32_t size){
        struct tm now_tm;

        time_t nows=time(NULL);
        localtime_r(&nows,&now_tm);
        snprintf(buffer,size,"%04d%02d%02d",
                 now_tm.tm_year+1900,now_tm.tm_mon+1,now_tm.tm_mday);

    }


    /**
     * @brief sleep_ms
     * @param sleep_time
     */
    static void Sleep_Millisecond(int sleep_time_ms)
    {
        usleep(sleep_time_ms*1000);
    }


    static timespec getDeltFromNow(timespec& value){

        struct timespec now;
        clock_gettime(CLOCK_REALTIME, &now);

        now.tv_sec = value.tv_sec -now.tv_sec;
        now.tv_nsec = value.tv_nsec - now.tv_nsec;

        return now;
    }

    static int64_t getSubtractMillisecond(timespec& a, timespec& b)
    {
        return (a.tv_sec - b.tv_sec )*1000 +(a.tv_nsec - b.tv_nsec)/1000000;
    }
};


/**
 * @brief The TimeMoniter class
 */
class TimeMoniter {
public:

#ifdef __x86_64__
    template<typename T1,typename... T2>
    TimeMoniter(unsigned int ms,T1 t1,T2... t2):_ms(ms)
    {

        start = TimerHelper::Now_Millisecond();
        Append(t1,t2...);

    }

    template<typename T1,typename... T2>
    void Append(T1 t1,T2... t2)
    {
        buf<<t1;
        Append(t2...);
    }
#elif __i386__

    template<typename T1>
    TimeMoniter(unsigned int ms,T1 t1){
        start = TimerHelper::Now_Millisecond();
        Append(t1);
    }
#endif
    template<typename T1>
    void Append(T1 t1)
    {
        buf<<t1;
    }

    ~TimeMoniter();

public:

    uint64_t start;
    unsigned int _ms;
    std::stringstream buf;
};

}
#endif // TIMER_H
