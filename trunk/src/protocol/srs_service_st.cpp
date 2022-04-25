#include <st.h>
#include <srs_kernel_error.hpp>
#include "srs_service_st.hpp"

srs_thread_t srs_thread_self() {
    return (srs_thread_t) st_thread_self();
}

srs_error_t srs_st_init() {
#ifdef __linux__
    // check epoll, some old linux donot support epoll.
    if (!srs_st_epoll_is_supported()) {
        return srs_error_new(ERROR_ST_SET_EPOLL, "linux epoll disabled");
    }
#endif

    // Select the best event system available on the OS. In Linux this is
    // epoll(). On BSD it will be kqueue.
    if (st_set_eventsys(ST_EVENTSYS_ALT) == -1) {
        return srs_error_new(ERROR_ST_SET_EPOLL, "st enable st failed, current is %s", st_get_eventsys_name());
    }

    // Before ST init, we might have already initialized the background cid.
    SrsContextId cid = _srs_context->get_id();
    if (cid.empty()) {
        cid = _srs_context->generate_id();
    }

    int r0 = 0;
    if ((r0 = st_init()) != 0) {
        return srs_error_new(ERROR_ST_INITIALIZE, "st initialize failed, r0=%d", r0);
    }


    // Switch to the background cid.
    _srs_context->set_id(cid);
    srs_info("st_init success, use %s", st_get_eventsys_name());

    return srs_success;
}

int srs_usleep(srs_utime_t usecs) {
    return st_usleep((st_utime_t) usecs);
}

srs_cond_t srs_cond_new() {
    return (srs_cond_t) st_cond_new();
}

int srs_cond_destroy(srs_cond_t cond) {
    return st_cond_destroy((st_cond_t) cond);
}

int srs_cond_signal(srs_cond_t cond) {
    return st_cond_signal((st_cond_t) cond);
}

int srs_cond_wait(srs_cond_t cond) {
    return st_cond_wait((st_cond_t) cond);
}