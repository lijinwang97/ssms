#ifndef SSMS_SRS_APP_HOURGLASS_HPP
#define SSMS_SRS_APP_HOURGLASS_HPP

#include <string>
#include <srs_core_time.hpp>
#include <vector>
#include "srs_app_st.hpp"

class ISrsFastTimer {
public:
    ISrsFastTimer();

    virtual ~ISrsFastTimer();

public:
    // Tick when timer is active.
    virtual srs_error_t on_timer(srs_utime_t interval) = 0;
};

// The fast timer, shared by objects, for high performance. 快速定时器，由对象共享，以实现高性能。
// For example, we should never start a timer for each connection or publisher or player,
// instead, we should start only one fast timer in server.
class SrsFastTimer : public ISrsCoroutineHandler{
private:
    SrsCoroutine* trd_;
    srs_utime_t interval_;
    std::vector<ISrsFastTimer*> handlers_;
public:
    SrsFastTimer(std::string label, srs_utime_t interval);
    virtual ~SrsFastTimer();

    // Interface ISrsCoroutineHandler
private:
    // Cycle the hourglass, which will sleep resolution every time.
    // and call handler when ticked.
    virtual srs_error_t cycle();
};

// To monitor the system wall clock timer deviation.
class SrsClockWallMonitor : public ISrsFastTimer
{
public:
    SrsClockWallMonitor();
    virtual ~SrsClockWallMonitor();
// interface ISrsFastTimer
private:
    srs_error_t on_timer(srs_utime_t interval);
};

// The handler for the tick.
class ISrsHourGlass
{
public:
    ISrsHourGlass();
    virtual ~ISrsHourGlass();
public:
    // When time is ticked, this function is called.
    virtual srs_error_t notify(int event, srs_utime_t interval, srs_utime_t tick) = 0;
};

#endif //SSMS_SRS_APP_HOURGLASS_HPP
