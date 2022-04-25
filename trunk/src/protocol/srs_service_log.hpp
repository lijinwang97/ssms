#ifndef SSMS_SRS_SERVICE_LOG_H
#define SSMS_SRS_SERVICE_LOG_H

#include <srs_kernel_log.hpp>
#include <srs_core.hpp>
#include <srs_app_reload.hpp>
#include <srs_kernel_kbps.hpp>
#include <map>
#include "srs_service_st.hpp"

SrsPps* _srs_pps_cids_get = NULL;
SrsPps* _srs_pps_cids_set = NULL;

class SrsThreadContext : public ISrsContext {

private:
    std::map<srs_thread_t, SrsContextId> cache;
public:
    SrsThreadContext();

    virtual ~SrsThreadContext();

public:
    virtual SrsContextId generate_id();

    virtual const SrsContextId &get_id();

    virtual const SrsContextId &set_id(const SrsContextId &v);

};

class SrsConsoleLog : public ISrsLog {
private:
    SrsLogLevel level;
    bool utc;
    char *buffer;
public:
    SrsConsoleLog(SrsLogLevel l, bool u);

    virtual ~SrsConsoleLog();

public:
    virtual void verbose(const char *tag, SrsContextId context_id, const char *fmt, ...);

    virtual void info(const char *tag, SrsContextId context_id, const char *fmt, ...);

    virtual void trace(const char *tag, SrsContextId context_id, const char *fmt, ...);

    virtual void warn(const char *tag, SrsContextId context_id, const char *fmt, ...);

    virtual void error(const char *tag, SrsContextId context_id, const char *fmt, ...);
};


//生成日志头。
//无论log是警告还是错误，如果为true，则记录errno。
//@param utc日志头是否使用utc时间格式。
//@param psize输出实际的头大小。
//这是一个内部API。
bool srs_log_header(char *buffer, int size, bool utc, bool dangerous, const char *tag,
                    SrsContextId cid, const char *level, int *psize);

#endif //SSMS_SRS_SERVICE_LOG_H
