#include "common/timerhelper.h"
#include "mio_timer.h"

#include "log/log.h"

namespace mtrpc {




AtomicIntegerT<uint32_t> TimerTask::TimerCount;


TimerTask::TimerTask(uint64_t starttimems,
                     uint64_t internalms,
                     Closure &t):
    callback(t),
    timer_id(TimerCount.addAndGet(1))
{
    value.tv_sec = starttimems / 1000;
    value.tv_nsec = (starttimems %1000) * 1000000;

    interval.tv_sec = internalms /1000;
    interval.tv_nsec = (internalms %1000) * 1000000;

    sprintf(name,
            "%u(%lu,%lu,%lu,%lu)",
            timer_id,
            value.tv_sec,value.tv_nsec,
            interval.tv_sec,interval.tv_nsec);

}


///
/// \brief TimerTask::nowValue
/// \return
///
struct timespec TimerTask::nowValue(){

    return value;
}

///
/// \brief deltValue
/// \return
///
struct timespec TimerTask::deltValue(){

    timespec ts = TimerHelper::getDeltFromNow(value);
    return ts;
}


///
/// \brief nowInterval
/// \return
///
struct timespec TimerTask::nowInterval(){

    return interval;
}

void TimerTask::updateToNext(){

    value.tv_nsec += interval.tv_nsec;

    value.tv_sec  +=
            interval.tv_sec + value.tv_nsec / 1000000000;

    value.tv_nsec =
            value.tv_nsec % 1000000000;


    sprintf(name,
            "timerid:%u,value:(%lu,%lu),interval:(%lu,%lu)",
            timer_id,
            value.tv_sec,value.tv_nsec,
            interval.tv_sec,interval.tv_nsec);

    //TRACE("update timer :"<<name);
}


bool TimerTask::isBefore(struct timespec& now)
{

    if(this->value.tv_sec < now.tv_sec)
        return true;
    else if(this->value.tv_sec > now.tv_sec)
        return false;

    if(this->value.tv_nsec < now.tv_nsec)
        return true;
    else if(this->value.tv_nsec > now.tv_nsec)
        return false;

    return false;
}


bool TimerTask::isRightRow(struct timespec& after)
{
    timespec delt = TimerHelper::getDeltFromNow(value);



    uint64_t diff =  TimerHelper::getSubtractMillisecond(after,delt);
    //TRACE("delt:"<<delt.tv_sec<<","          <<delt.tv_nsec<<",after:"<< after.tv_sec<<","<<after.tv_nsec<<",diff:"<<diff);

    return diff == 0 ;
}


//the smaller the top
bool TimerTaskCmp::operator () (const TimerTask* t1, const TimerTask* t2){


    if(t1->value.tv_sec < t2->value.tv_sec)
    {
        return false;
    }
    else if(t1->value.tv_sec > t2->value.tv_sec)
    {
        return true;
    }

    if(t1->value.tv_nsec < t2->value.tv_nsec)
    {
        return false;
    }
    else if(t1->value.tv_nsec > t2->value.tv_nsec)
    {
        return true;
    }

    return false;
}

EventTimer::EventTimer()
{
    ev._fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
    ev.ev.data.ptr = &ev;

    ev.onEvent = Closure::From<EventTimer,Epoller*,uint32_t, &EventTimer::onTimer>(this);
}

EventTimer::~EventTimer(){
    ::close(ev._fd);
}




///
/// \brief runTimerAt
/// \param starttimems
/// \param internalms
/// \param fun
/// \return
///
uint32_t EventTimer::runTimerAt(uint64_t starttimems,
                                uint64_t internalms,
                                Closure &t)
{

    uint64_t nowms = TimerHelper::Now_Millisecond();
    // adjust the the start time
    if(internalms > 0
            && nowms > starttimems )
    {
        uint64_t delt = nowms - starttimems;
        starttimems += (delt/internalms +1) *internalms;
    }

    TimerTask* tt =new TimerTask(starttimems, internalms,t);
    timertasks.push(tt);
    timerindex.insert(std::make_pair(tt->timer_id, tt));

    updateTimer();

    return tt->timer_id;
}


///
/// \brief runTimerAfter
/// \param starttimems
/// \param internalms
/// \param fun
/// \return
///
uint32_t EventTimer::runTimerAfter(uint64_t starttimems,
                                   uint64_t internalms,
                                   Closure &t){

    starttimems = starttimems + TimerHelper::Now_Millisecond();

    TimerTask* tt = new TimerTask(starttimems, internalms, t);
    timertasks.push(tt);
    timerindex.insert(std::make_pair(tt->timer_id, tt));

    updateTimer();

    return tt->timer_id;
}


///
/// \brief stopTimer
/// \param timerid
/// \return
///
int EventTimer::stopTimer(uint32_t timerid){

    std::map<int, TimerTask*>::iterator it = timerindex.find(timerid);

    if(it == timerindex.end() )
        return -1;

    TimerTask* tt = it->second;


    // is next timer task
    if(timertasks.top() == tt)
    {
        timertasks.pop();
        delete tt;

    }else{
        // delete on time out
        tt->stop();
    }

    timerindex.erase(tt->timer_id);

    return 0;
}




///
/// \brief onTimer
///
void EventTimer::onTimer(Epoller* p,uint32_t events)
{
    if(timertasks.empty())
    {
        printf("no timer task\n");
        return ;
    }

    struct timespec now;
    clock_gettime(CLOCK_REALTIME,&now);

    while (!timertasks.empty()) {

        TimerTask* sp = timertasks.top();

        if(!sp->isBefore(now))
        {
           // printf("error sp:%s now:%lu,%lu\n",sp->name,now.tv_sec,now.tv_nsec);
            break;
        }

        timertasks.pop();

        sp->callback();

        if(sp->isPeriod())
        {

            sp->updateToNext();
            timertasks.push(sp);
        }
        else
        {
            delete sp;

        }
    }

    updateTimer();
}
///
/// \brief updateTimer
/// \return
///
int EventTimer::updateTimer()
{
    // if no task disable
    if(timertasks.empty())
    {
        struct itimerspec null ={{0,0},{0,0}};
        //TRACE("set next timer:("<<null.it_value.tv_sec<<","<<null.it_value.tv_nsec<<")");
        return ::timerfd_settime(ev._fd, TFD_TIMER_ABSTIME,&null,NULL);
    }

    struct itimerspec next;

    next.it_value = timertasks.top()->nowValue();
    next.it_interval= timertasks.top()->nowInterval();

    // set new timer
    return ::timerfd_settime(ev._fd, TFD_TIMER_ABSTIME, &next, NULL);

}

}//name space



