#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sstream>
#include <unistd.h>

#include <srs_core.hpp>
#include <srs_kernel_error.hpp>
#include <srs_kernel_log.hpp>
#include <srs_app_config.hpp>
#include <srs_app_threads.hpp>
#include <srs_app_hybrid.hpp>

using namespace std;

// pre-declare
srs_error_t run_directly_or_daemon();

srs_error_t srs_detect_docker();

srs_error_t run_hybrid_server();

void show_macro_features();

// @global log and context.
ISrsLog *_srs_log = NULL;
ISrsContext *_srs_context = NULL;
// @global config object for app module.
SrsConfig *_srs_config = NULL;

srs_error_t run_hybrid_server() {
    srs_error_t err = srs_success;

    // Create servers and register them. 创建服务器并注册
    _srs_hybrid->register_server(new SrsServerAdapter());

#ifdef SRS_SRT
    _srs_hybrid->register_server(new SrtServerAdapter());
#endif

#ifdef SRS_RTC
    _srs_hybrid->register_server(new RtcServerAdapter());
#endif

    // Do some system initialize.
    if ((err = _srs_hybrid->initialize()) != srs_success) {
        return srs_error_wrap(err, "hybrid initialize");
    }

    // Circuit breaker to protect server, which depends on hybrid.
    if ((err = _srs_circuit_breaker->initialize()) != srs_success) {
        return srs_error_wrap(err, "init circuit breaker");
    }

    // Should run util hybrid servers all done.
    if((err = _srs_hybrid->run())!=srs_success){
        return srs_error_wrap(err, "hybrid run");
    }

    // After all done, stop and cleanup.
    _srs_hybrid->stop();

    return err;


}

srs_error_t run_directly_or_daemon() {
    srs_error_t err = srs_success;

    // Try to load the config if docker detect failed.
    if (!_srs_in_docker) {
        _srs_in_docker = _srs_config->get_in_docker();
        if (_srs_in_docker) {
            srs_trace("enable in_docker by config");
        }
    }

    // Load daemon from config, disable it for docker.
    // @see https://github.com/ossrs/srs/issues/1594
    bool run_as_daemon = _srs_config->get_daemon();
    if (run_as_daemon && _srs_in_docker && _srs_config->disable_daemon_for_docker()) {
        srs_warn("disable daemon for docker");
        run_as_daemon = false;
    }

    // If not daemon, directly run hybrid server.
    if (!run_as_daemon) {
        if ((err = run_hybrid_server()) != srs_success) {
            return srs_error_wrap(err, "run hybrid");
        }
        return srs_success;
    }

}

srs_error_t do_main(int argc, char **argv) {
    srs_error_t err = srs_success;

    // Initialize global or thread-local variables.
    if((err = srs_thread_initialize())!=srs_success){
        return srs_error_wrap(err,"thread init");
    }

    // For background context id.
    _srs_context->set_id(_srs_context->generate_id());

    // TODO: support both little and big endian.
    srs_assert(srs_is_little_endian());

    // for gperf gmp or gcp,
    // should never enable it when not enabled for performance issue.
#ifdef SRS_GPERF_MP
    HeapProfilerStart("gperf.srs.gmp");
#endif
#ifdef SRS_GPERF_CP
    ProfilerStart("gperf.srs.gcp");
#endif

    // never use gmp to check memory leak.
#ifdef SRS_GPERF_MP
#warning "gmp is not used for memory leak, please use gmc instead."
#endif

    // Ignore any error while detecting docker.
    if ((err = srs_detect_docker()) != srs_success) {
        srs_error_reset(err);
    }

    // never use srs log(srs_trace, srs_error, etc) before config parse the option,
    // which will load the log config and apply it. 在配置解析选项之前，永远不要使用SRS日志(srs_trace, srs_error等)，它将加载日志配置并应用它。
    if ((err = _srs_config->parse_options(argc, argv)) != srs_success) {
        return srs_error_wrap(err, "config parse options");
    }

    // change the work dir and set cwd.
    int r0 = 0;
    string cwd = _srs_config->get_work_dir();
    if (!cwd.empty() && cwd != "./" && (r0 = chdir(cwd.c_str())) == -1) {
        return srs_error_new(-1, "chdir to %s, r0=%d", cwd.c_str(), r0);
    }
    if ((err = _srs_config->initialize_cwd()) != srs_success) {
        return srs_error_wrap(err, "config cwd");
    }

    // config parsed, initialize log.
    if ((err = _srs_log->initialize()) != srs_success) {
        return srs_error_wrap(err, "log initialize");
    }

    // config already applied to log.
    srs_trace2(TAG_MAIN, "%s, %s", RTMP_SIG_SRS_SERVER, RTMP_SIG_SRS_LICENSE);
    srs_trace("authors: %s", RTMP_SIG_SRS_AUTHORS);
    srs_trace("contributors: %s", SRS_CONSTRIBUTORS);
    srs_trace("cwd=%s, work_dir=%s, build: %s, configure: %s, uname: %s, osx: %d, pkg: %s",
              _srs_config->cwd().c_str(), cwd.c_str(), SRS_BUILD_DATE, SRS_USER_CONFIGURE, SRS_UNAME, SRS_OSX_BOOL,
              SRS_PACKAGER);
    srs_trace("configure detail: " SRS_CONFIGURE);
#ifdef SRS_EMBEDED_TOOL_CHAIN
    srs_trace("crossbuild tool chain: " SRS_EMBEDED_TOOL_CHAIN);
#endif

    // for memory check or detect.
    if (true) {
        stringstream ss;

#ifdef SRS_PERF_GLIBC_MEMORY_CHECK
        // ensure glibc write error to stderr.
        string lfsov = srs_getenv("LIBC_FATAL_STDERR_");
        setenv("LIBC_FATAL_STDERR_", "1", 1);
        string lfsnv = srs_getenv("LIBC_FATAL_STDERR_");
        //
        // ensure glibc to do alloc check.
        string mcov = srs_getenv("MALLOC_CHECK_");
        setenv("MALLOC_CHECK_", "1", 1);
        string mcnv = srs_getenv("MALLOC_CHECK_");
        ss << "glic mem-check env MALLOC_CHECK_ " << mcov << "=>" << mcnv << ", LIBC_FATAL_STDERR_ " << lfsov << "=>" << lfsnv << ".";
#endif

#ifdef SRS_GPERF_MC
        string hcov = srs_getenv("HEAPCHECK");
        if (hcov.empty()) {
            string cpath = _srs_config->config();
            srs_warn("gmc HEAPCHECK is required, for example: env HEAPCHECK=normal ./objs/srs -c %s", cpath.c_str());
        } else {
            ss << "gmc env HEAPCHECK=" << hcov << ".";
        }
#endif

#ifdef SRS_GPERF_MD
        char* TCMALLOC_PAGE_FENCE = getenv("TCMALLOC_PAGE_FENCE");
        if (!TCMALLOC_PAGE_FENCE || strcmp(TCMALLOC_PAGE_FENCE, "1")) {
            srs_warn("gmd enabled without env TCMALLOC_PAGE_FENCE=1");
        } else {
            ss << "gmd env TCMALLOC_PAGE_FENCE=" << TCMALLOC_PAGE_FENCE << ".";
        }
#endif

        string sss = ss.str();
        if (!sss.empty()) {
            srs_trace(sss.c_str());
        }
    }

    // we check the config when the log initialized.
    if ((err = _srs_config->check_config()) != srs_success) {
        return srs_error_wrap(err, "check config");
    }

    // features
    show_macro_features();

#ifdef SRS_GPERF
    // For tcmalloc, use slower release rate.
    if (true) {
        double trr = _srs_config->tcmalloc_release_rate();
        double otrr = MallocExtension::instance()->GetMemoryReleaseRate();
        MallocExtension::instance()->SetMemoryReleaseRate(trr);
        srs_trace("tcmalloc: set release-rate %.2f=>%.2f", otrr, trr);
    }
#endif

    if ((err = run_directly_or_daemon()) != srs_success) {
        return srs_error_wrap(err, "run");
    }

    return err;

}



int main(int argc, char **argv) {
    srs_error_t err = do_main(argc, argv);

    if (err != srs_success) {
        srs_error("Failed, %s", srs_error_desc(err).c_str());
    }

    int ret = srs_error_code(err);
    srs_freep(err);

    return ret;
}


