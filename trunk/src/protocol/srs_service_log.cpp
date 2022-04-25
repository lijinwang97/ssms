#include <cstdlib>
#include <sys/time.h>
#include <cerrno>
#include <unistd.h>
#include "srs_service_log.hpp"

#define SRS_BASIC_LOG_SIZE 8192

SrsConsoleLog::~SrsConsoleLog() {
    srs_freepa(buffer);
}

SrsConsoleLog::SrsConsoleLog(SrsLogLevel l, bool u) {
    level = l;
    utc = u;
    buffer = new char[SRS_BASIC_LOG_SIZE];
}

void SrsConsoleLog::error(const char *tag, SrsContextId context_id, const char *fmt, ...) {
    if (level > SrsLogLevelError) {
        return;
    }

    int size = 0;
    if (!srs_log_header(buffer, SRS_BASIC_LOG_SIZE, utc, true, tag, context_id, "Error", &size)) {
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    size += vsnprintf(buffer + size, SRS_BASIC_LOG_SIZE - size, fmt, ap);
    va_end(ap);

    // add strerror() to error msg.
    if (errno != 0) {
        size += snprintf(buffer + size, SRS_BASIC_LOG_SIZE - size, "(%s)", strerror(errno));
    }

    fprintf(stderr, "%s\n", buffer);
}

void SrsConsoleLog::warn(const char *tag, SrsContextId context_id, const char *fmt, ...) {
    if (level > SrsLogLevelWarn) {
        return;
    }

    int size = 0;
    if (!srs_log_header(buffer, SRS_BASIC_LOG_SIZE, utc, true, tag, context_id, "Warn", &size)) {
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    // we reserved 1 bytes for the new line.
    size += vsnprintf(buffer + size, SRS_BASIC_LOG_SIZE - size, fmt, ap);
    va_end(ap);

    fprintf(stderr, "%s\n", buffer);
}

void SrsConsoleLog::trace(const char *tag, SrsContextId context_id, const char *fmt, ...) {
    if (level > SrsLogLevelTrace) {
        return;
    }

    int size = 0;
    if (!srs_log_header(buffer, SRS_BASIC_LOG_SIZE, utc, false, tag, context_id, "Trace", &size)) {
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    // we reserved 1 bytes for the new line.
    size += vsnprintf(buffer + size, SRS_BASIC_LOG_SIZE - size, fmt, ap);
    va_end(ap);

    fprintf(stdout, "%s\n", buffer);
}

void SrsConsoleLog::info(const char *tag, SrsContextId context_id, const char *fmt, ...) {
    if (level > SrsLogLevelInfo) {
        return;
    }

    int size = 0;
    if (!srs_log_header(buffer, SRS_BASIC_LOG_SIZE, utc, false, tag, context_id, "Debug", &size)) {
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    // we reserved 1 bytes for the new line.
    size += vsnprintf(buffer + size, SRS_BASIC_LOG_SIZE - size, fmt, ap);
    va_end(ap);

    fprintf(stdout, "%s\n", buffer);
}

void SrsConsoleLog::verbose(const char *tag, SrsContextId context_id, const char *fmt, ...) {
    if (level > SrsLogLevelVerbose) {
        return;
    }

    int size = 0;
    if (!srs_log_header(buffer, SRS_BASIC_LOG_SIZE, utc, false, tag, context_id, "Verb", &size)) {
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    // we reserved 1 bytes for the new line.
    size += vsnprintf(buffer + size, SRS_BASIC_LOG_SIZE - size, fmt, ap);
    va_end(ap);

    fprintf(stdout, "%s\n", buffer);
}

bool srs_log_header(char *buffer, int size, bool utc, bool dangerous, const char *tag,
                    SrsContextId cid, const char *level, int *psize) {
    // clock time;
    timeval tv;
    if (gettimeofday(&tv, NULL) == -1) {
        return false;
    }

    // to calendar time 日历时间
    struct tm now;
    // Each of these functions returns NULL in case an error was detected. @see https://linux.die.net/man/3/localtime_r
    if (utc) {
        if (gmtime_r(&tv.tv_sec, &now) == NULL) {
            return false;
        }
    } else {
        if (localtime_r(&tv.tv_sec, &now) == NULL) {
            return false;
        }
    }

    int written = -1;
    if (dangerous) {
        if (tag) {
            written = snprintf(buffer, size,
                               "[%d-%02d-%02d %02d:%02d:%02d.%03d][%s][%d][%s][%d][%s] ",
                               1900 + now.tm_year, 1 + now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec,
                               (int) (tv.tv_usec / 1000),
                               level, getpid(), cid.c_str(), errno, tag);
        } else {
            written = snprintf(buffer, size,
                               "[%d-%02d-%02d %02d:%02d:%02d.%03d][%s][%d][%s][%d] ",
                               1900 + now.tm_year, 1 + now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec,
                               (int) (tv.tv_usec / 1000),
                               level, getpid(), cid.c_str(), errno);
        }
    } else {
        if (tag) {
            written = snprintf(buffer, size,
                               "[%d-%02d-%02d %02d:%02d:%02d.%03d][%s][%d][%s][%s] ",
                               1900 + now.tm_year, 1 + now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec,
                               (int) (tv.tv_usec / 1000),
                               level, getpid(), cid.c_str(), tag);
        } else {
            written = snprintf(buffer, size,
                               "[%d-%02d-%02d %02d:%02d:%02d.%03d][%s][%d][%s] ",
                               1900 + now.tm_year, 1 + now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec,
                               (int) (tv.tv_usec / 1000),
                               level, getpid(), cid.c_str());
        }
    }

    if (written >= size) {
        return false;
    }

    if (written == -1) {
        return false;
    }

    // write the header size.
    *psize = written;

    return true;
}

SrsThreadContext::SrsThreadContext() {

}

SrsThreadContext::~SrsThreadContext() {

}

SrsContextId SrsThreadContext::generate_id() {
    return SrsContextId();
}

const SrsContextId &SrsThreadContext::get_id() {
    ++_srs_pps_cids_get->sugar;
    return cache[srs_thread_self()];
}

const SrsContextId &SrsThreadContext::set_id(const SrsContextId &v) {
    ++_srs_pps_cids_set->sugar;
    srs_thread_t self = srs_thread_self();

    if (cache.find(self) == cache.end()) {
        cache[self] = v;
        return v;
    }

    const SrsContextId &ov = cache[self];
    cache[self] = v;
    return ov;
}
