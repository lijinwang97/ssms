#ifndef SSMS_SRS_SERVICE_ST_HPP
#define SSMS_SRS_SERVICE_ST_HPP

#include <srs_core.hpp>
#include <srs_core_time.hpp>

// Wrap for coroutine.
typedef void* srs_thread_t;
typedef void* srs_cond_t;
typedef void* srs_mutex_t;
typedef void* srs_cond_t;

extern srs_thread_t srs_thread_self();

extern srs_error_t srs_st_init();

extern int srs_usleep(srs_utime_t usecs);

// Wrap for coroutine.
extern srs_cond_t srs_cond_new();

extern int srs_cond_destroy(srs_cond_t cond);

extern int srs_cond_signal(srs_cond_t cond);

extern int srs_cond_wait(srs_cond_t cond);

#endif //SSMS_SRS_SERVICE_ST_HPP
