#ifndef SSMS_SRS_KERNEL_LOG_H
#define SSMS_SRS_KERNEL_LOG_H

#include <srs_core.hpp>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <string>

enum SrsLogLevel {
    SrsLogLevelForbidden = 0x00,
    // Only used for very verbose debug, generally,
    // we compile without this level for high performance.
    SrsLogLevelVerbose = 0x01,
    SrsLogLevelInfo = 0x02,
    SrsLogLevelTrace = 0x04,
    SrsLogLevelWarn = 0x08,
    SrsLogLevelError = 0x10,
    SrsLogLevelDisabled = 0x20,
};

class ISrsLog {
public:
    ISrsLog();

    virtual ~ISrsLog();

public:
    // The log for verbose, very verbose information.
    virtual void verbose(const char *tag, SrsContextId context_id, const char *fmt, ...) = 0;

    // The log for debug, detail information.
    virtual void info(const char *tag, SrsContextId context_id, const char *fmt, ...) = 0;

    // The log for trace, important information.
    virtual void trace(const char *tag, SrsContextId context_id, const char *fmt, ...) = 0;

    // The log for warn, warn is something should take attention, but not a error.
    virtual void warn(const char *tag, SrsContextId context_id, const char *fmt, ...) = 0;

    //错误日志，某些错误发生，对错误做一些处理，例如。关闭连接，但我们不会中止程序。
    virtual void error(const char *tag, SrsContextId context_id, const char *fmt, ...) = 0;

};

class ISrsContext {
public:
    ISrsContext();

    virtual ~ISrsContext();

public:
    // Generate a new context id. 生成一个新的上下文id。
    // @remark We do not set to current thread, user should do this.
    virtual SrsContextId generate_id() = 0;
    // Get the context id of current thread.
    virtual const SrsContextId& get_id() = 0;

    virtual const SrsContextId& set_id(const SrsContextId& v) = 0;

};


// @global User must provides a log object
extern ISrsLog *_srs_log;
// @global User must implements the LogContext and define a global instance.
extern ISrsContext *_srs_context;

#define srs_verbose(msg, ...) _srs_log->verbose(NULL, _srs_context->get_id(), msg, ##__VA_ARGS__)
#define srs_info(msg, ...)    _srs_log->info(NULL, _srs_context->get_id(), msg, ##__VA_ARGS__)
#define srs_trace(msg, ...)   _srs_log->trace(NULL, _srs_context->get_id(), msg, ##__VA_ARGS__)
#define srs_warn(msg, ...)    _srs_log->warn(NULL, _srs_context->get_id(), msg, ##__VA_ARGS__)
#define srs_error(msg, ...)   _srs_log->error(NULL, _srs_context->get_id(), msg, ##__VA_ARGS__)


#endif //SSMS_SRS_KERNEL_LOG_H
