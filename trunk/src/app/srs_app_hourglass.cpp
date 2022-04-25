#include <srs_kernel_log.hpp>
#include <srs_kernel_kbps.hpp>
#include <srs_kernel_utility.hpp>
#include "srs_app_hourglass.hpp"


SrsPps *_srs_pps_timer = NULL;
SrsPps* _srs_pps_conn = NULL;
SrsPps* _srs_pps_pub = NULL;

extern SrsPps* _srs_pps_clock_15ms;
extern SrsPps* _srs_pps_clock_20ms;
extern SrsPps* _srs_pps_clock_25ms;
extern SrsPps* _srs_pps_clock_30ms;
extern SrsPps* _srs_pps_clock_35ms;
extern SrsPps* _srs_pps_clock_40ms;
extern SrsPps* _srs_pps_clock_80ms;
extern SrsPps* _srs_pps_clock_160ms;
extern SrsPps* _srs_pps_timer_s;

ISrsFastTimer::ISrsFastTimer() {

}

ISrsFastTimer::~ISrsFastTimer() {

}

SrsFastTimer::SrsFastTimer(std::string label, srs_utime_t interval) {
    interval_ = interval;
    trd_ = new SrsSTCoroutine(label, this, _srs_context->get_id());
}

SrsFastTimer::~SrsFastTimer() {
    srs_freep(trd_);
}

srs_error_t SrsFastTimer::cycle() {
    srs_error_t err = srs_success;

    while (true) {
        if ((err = trd_->pull()) != srs_success) {
            return srs_error_wrap(err, "quit");
        }

        ++_srs_pps_timer->sugar;

        for (int i = 0; i < (int) handlers_.size(); i++) {
            ISrsFastTimer *timer = handlers_.at(i);

            if ((err = timer->on_timer(interval_)) != srs_success) {
                srs_freep(err); // Ignore any error for shared timer.
            }
        }

        srs_usleep(interval_);
    }

    return err;
}

SrsClockWallMonitor::SrsClockWallMonitor() {

}

SrsClockWallMonitor::~SrsClockWallMonitor() {

}

srs_error_t SrsClockWallMonitor::on_timer(srs_utime_t interval) {
    srs_error_t err = srs_success;

    static srs_utime_t clock = 0;

    srs_utime_t now = srs_update_system_time();
    if (!clock) {
        clock = now;
        return err;
    }

    srs_utime_t elapsed = now - clock;
    clock = now;

    if (elapsed <= 15 * SRS_UTIME_MILLISECONDS) {
        ++_srs_pps_clock_15ms->sugar;
    } else if (elapsed <= 21 * SRS_UTIME_MILLISECONDS) {
        ++_srs_pps_clock_20ms->sugar;
    } else if (elapsed <= 25 * SRS_UTIME_MILLISECONDS) {
        ++_srs_pps_clock_25ms->sugar;
    } else if (elapsed <= 30 * SRS_UTIME_MILLISECONDS) {
        ++_srs_pps_clock_30ms->sugar;
    } else if (elapsed <= 35 * SRS_UTIME_MILLISECONDS) {
        ++_srs_pps_clock_35ms->sugar;
    } else if (elapsed <= 40 * SRS_UTIME_MILLISECONDS) {
        ++_srs_pps_clock_40ms->sugar;
    } else if (elapsed <= 80 * SRS_UTIME_MILLISECONDS) {
        ++_srs_pps_clock_80ms->sugar;
    } else if (elapsed <= 160 * SRS_UTIME_MILLISECONDS) {
        ++_srs_pps_clock_160ms->sugar;
    } else {
        ++_srs_pps_timer_s->sugar;
    }

    return err;
}

ISrsHourGlass::ISrsHourGlass() {

}

ISrsHourGlass::~ISrsHourGlass() {

}
