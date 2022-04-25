#include <srs_kernel_error.hpp>
#include <srs_service_log.hpp>
#include "srs_app_threads.hpp"
#include "srs_app_log.hpp"
#include "srs_app_config.hpp"
#include "srs_app_hybrid.hpp"

extern SrsPps* _srs_pps_timer;

srs_error_t srs_thread_initialize(){
    srs_error_t err = srs_success;

    // Root global objects.
    _srs_log = new SrsFileLog();
    _srs_context = new SrsThreadContext();
    _srs_config = new SrsConfig();

    // The clock wall object.
    _srs_clock = new SrsWallClock();

    // The pps cids depends by st init.
    _srs_pps_cids_get = new SrsPps();
    _srs_pps_cids_set = new SrsPps();

    // Initialize ST, which depends on pps cids.
    if ((err = srs_st_init())!= srs_success){
        return srs_error_wrap(err,"initialize st failed");
    }

    // The global objects which depends on ST.
    _srs_hybrid = new SrsHybridServer();
    _srs_sources = new SrsLiveSourceManager();
    _srs_stages = new SrsStageManager();
    _srs_circuit_breaker = new SrsCircuitBreaker();

}