#ifndef SSMS_SRS_KERNEL_UTILITY_HPP
#define SSMS_SRS_KERNEL_UTILITY_HPP

#include <srs_core_time.hpp>
#include "string"

#define srs_min(a, b) (((a) < (b))? (a) : (b))
#define srs_max(a, b) (((a) < (b))? (b) : (a))



// A daemon st-thread updates it.
extern srs_utime_t srs_update_system_time();
// Get current system time in srs_utime_t, use cache to avoid performance problem
extern srs_utime_t srs_get_system_time();
// Parse the int64 value to string.
extern std::string srs_int2str(int64_t value);

// For utest to mock it.
#include <sys/time.h>
#ifdef SRS_OSX
#define _srs_gettimeofday gettimeofday
#else
typedef int (*srs_gettimeofday_t) (struct timeval* tv, struct timezone* tz);
#endif

#endif //SSMS_SRS_KERNEL_UTILITY_HPP
