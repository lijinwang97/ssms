#ifndef SSMS_SRS_APP_LOG_HPP
#define SSMS_SRS_APP_LOG_HPP

#include <srs_kernel_log.hpp>
#include "srs_app_reload.hpp"
#include <srs_kernel_utility.hpp>

// Use memory/disk cache and donot flush when write log.
// it's ok to use it without config, which will log to console, and default trace level.
// when you want to use different level, override this classs, set the protected _level.
class SrsFileLog : public ISrsLog, public ISrsReloadHandler {
private:
    // Defined in SrsLogLevel.
    SrsLogLevel level;

private:
    char* log_data;
    //Log to file if specified srs_log_file
    int fd;
    // Whether log to file tank
    bool log_to_file_tank;
    // Whether use utc time. 协调世界时
    bool utc;
public:
    SrsFileLog();

    virtual ~SrsFileLog();

public:
    virtual void verbose(const char *tag, SrsContextId context_id, const char *fmt, ...);

    virtual void info(const char *tag, SrsContextId context_id, const char *fmt, ...);

    virtual void trace(const char *tag, SrsContextId context_id, const char *fmt, ...);

    virtual void warn(const char *tag, SrsContextId context_id, const char *fmt, ...);

    virtual void error(const char *tag, SrsContextId context_id, const char *fmt, ...);

private:
    virtual void write_log(int& fd, char* srs_log,int size,int level);
    virtual void open_log_file();
};

#endif //SSMS_SRS_APP_LOG_HPP
