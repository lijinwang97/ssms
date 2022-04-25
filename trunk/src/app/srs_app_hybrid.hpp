#ifndef SSMS_SRS_APP_HYBRID_HPP
#define SSMS_SRS_APP_HYBRID_HPP

#include <srs_kernel_kbps.hpp>
#include <srs_app_hourglass.hpp>
#include <srs_app_utility.hpp>
#include <srs_app_server.hpp>

using namespace std;

extern SrsPps *_srs_pps_cids_get;
extern SrsPps *_srs_pps_cids_set;

class SrsServer;

// The hibrid server interfaces, we could register many servers.
class ISrsHybridServer
{
public:
    ISrsHybridServer();
    virtual ~ISrsHybridServer();
public:
    // Only ST initialized before each server, we could fork processes as such.
    virtual srs_error_t initialize() = 0;
    // Run each server, should never block except the SRS master server.
    virtual srs_error_t run(SrsWaitGroup* wg) = 0;
    // Stop each server, should do cleanup, for example, kill processes forked by server.
    virtual void stop() = 0;
};


// The hybrid server manager. 混合服务器管理器。
class SrsHybridServer : public ISrsFastTimer {
private:
    std::vector<ISrsHybridServer*> servers;
    SrsFastTimer *timer20ms_;
    SrsFastTimer *timer100ms_;
    SrsFastTimer *timer1s_;
    SrsFastTimer *timer5s_;
    SrsClockWallMonitor* clock_monitor_;
public:
    SrsHybridServer();

    virtual ~SrsHybridServer();

public:
    virtual void register_server(ISrsHybridServer* svr);
public:
    virtual srs_error_t initialize();
    virtual srs_error_t run();
    virtual void stop();

// interface ISrsFastTimer
private:
    srs_error_t on_timer(srs_utime_t interval);
};

class SrsServerAdapter : public ISrsHybridServer{
private:
    SrsServer* srs;
public:
    SrsServerAdapter();
    virtual ~SrsServerAdapter();
public:
    virtual srs_error_t initialize();
    virtual srs_error_t run(SrsWaitGroup* wg);
    virtual void stop();
public:
    virtual SrsServer* instance();

};

extern SrsHybridServer* _srs_hybrid;

#endif //SSMS_SRS_APP_HYBRID_HPP
