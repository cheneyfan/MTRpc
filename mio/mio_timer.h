#ifndef __MIO_TIMER_H_
#define __MIO_TIMER_H_

#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/timerfd.h>

#include <map>
#include <queue>


#include "atomic.h"
#include "mio_task.h"
#include "mio_event.h"


namespace  mio2 {

class TimerTask{
public:
    TimerTask(uint64_t starttimems,
              uint64_t internalms,
              Closure& t);
    ///
    /// \brief nextTimer
    /// \return
    ///
    struct timespec nowValue();

    ///
    /// \brief deltValue
    /// \return
    ///
    struct timespec deltValue();

    ///
    /// \brief nowInterval
    /// \return
    ///
    struct timespec nowInterval();

    ///
    /// \brief updateToNext
    ///
    void updateToNext();

    ///
    /// \brief isPeriod
    /// \return
    ///
    bool isPeriod(){
        return interval.tv_nsec != 0
                || interval.tv_sec != 0;
    }

    void stop(){
        interval.tv_nsec = 0;
        interval.tv_sec = 0;
    }
    ///
    /// \brief isBefore
    /// \param now
    /// \return
    ///
    bool isBefore(struct timespec& now);

    ///
    /// \brief isRightRow
    /// \param after
    /// \return
    ///
    bool isRightRow(struct timespec& after);

public:
    static AtomicIntegerT<uint32_t> TimerCount;

public:
    struct timespec value;
    struct timespec interval;

    Closure callback;
    uint32_t timer_id;
    char name[64];
};


class TimerTaskCmp {
public:
    bool operator ()(const TimerTask* t1,const TimerTask* t2);
};


class EventTimer {
public:

    EventTimer();
    ~EventTimer();



     void onEvent(Epoller* p,uint32_t events);



    ///
    /// \brief runTimerAt
    /// \param starttimems
    /// \param internalms
    /// \param fun
    /// \return
    ///
    uint32_t runTimerAt(uint64_t starttimems,
                        uint64_t internalms,
                        Closure &t);


    ///
    /// \brief runTimerAfter
    /// \param starttimems
    /// \param internalms
    /// \param fun
    /// \return
    ///
    uint32_t runTimerAfter(uint64_t starttimems,
                           uint64_t internalms,
                           Closure &t);

    ///
    /// \brief stopTimer
    /// \param timerid
    /// \return
    ///
    int stopTimer(uint32_t timerid);


    ///
    /// \brief onTimer
    ///
    void onTimer(Epoller* p,uint32_t events);

    ///
    /// \brief updateTimer
    /// \return
    ///
    int updateTimer();


public:
    std::priority_queue<TimerTask*,
    std::vector<TimerTask*>, TimerTaskCmp> timertasks;
    std::map<int, TimerTask*> timerindex;
public:
    IOEvent ev;

};


}
#endif
