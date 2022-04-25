#include <cstdlib>
#include "srs_kernel_utility.hpp"
#include "srs_kernel_log.hpp"
#include "inttypes.h"

srs_utime_t _srs_system_time_us_cache = 0;
srs_utime_t _srs_system_time_startup_time = 0;

// this value must:
// equals to (SRS_SYS_CYCLE_INTERVAL*SRS_SYS_TIME_RESOLUTION_MS_TIMES)*1000
// @see SRS_SYS_TIME_RESOLUTION_MS_TIMES
#define SYS_TIME_RESOLUTION_US 300*1000


srs_utime_t srs_update_system_time() {
    timeval now;

    if (_srs_gettimeofday(&now, NULL) < 0) {
        srs_warn("gettimeofday failed, ignore");
        return -1;
    }

    // we must convert the tv_sec/tv_usec to int64_t.
    int64_t now_us = ((int64_t) now.tv_sec) * 1000 * 1000 + (int64_t) now.tv_usec;

    // for some ARM os, the starttime maybe invalid,
    // for example, on the cubieboard2, the srs_startup_time is 1262304014640,
    // while now is 1403842979210 in ms, diff is 141538964570 ms, 1638 days
    // it's impossible, and maybe the problem of startup time is invalid.
    // use date +%s to get system time is 1403844851.
    // so we use relative time.
    if (_srs_system_time_us_cache <= 0) {
        _srs_system_time_startup_time = _srs_system_time_us_cache = now_us;
        return _srs_system_time_us_cache;
    }

    // use relative time.
    int64_t diff = now_us - _srs_system_time_us_cache;
    diff = srs_max(0, diff);
    if (diff < 0 || diff > 1000 * SYS_TIME_RESOLUTION_US) {
        srs_warn("clock jump, history=%" PRId64 "us, now=%" PRId64 "us, diff=%" PRId64 "us", _srs_system_time_us_cache,
                 now_us, diff);
        _srs_system_time_startup_time += diff;
    }

    _srs_system_time_us_cache = now_us;
    srs_info("clock updated, startup=%" PRId64 "us, now=%" PRId64 "us", _srs_system_time_startup_time,
             _srs_system_time_us_cache);

    return _srs_system_time_us_cache;
}

srs_utime_t srs_get_system_time() {
    if (_srs_system_time_us_cache <= 0) {
        srs_update_system_time();
    }

    return _srs_system_time_us_cache;
}


std::string srs_int2str(int64_t value)
{
    // len(max int64_t) is 20, plus one "+-."
    char tmp[22];
    snprintf(tmp, 22, "%" PRId64, value);
    return tmp;
}