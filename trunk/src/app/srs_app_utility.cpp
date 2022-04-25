#include <cstring>
#include "srs_app_utility.hpp"

static SrsProcSelfStat _srs_system_cpu_self_stat;

SrsProcSelfStat::SrsProcSelfStat() {
    ok = false;
    sample_time = 0;
    percent = 0;

    pid = 0;
    memset(comm, 0, sizeof(comm));
    state = '0';
    ppid = 0;
    pgrp = 0;
    session = 0;
    tty_nr = 0;
    tpgid = 0;
    flags = 0;
    minflt = 0;
    cminflt = 0;
    majflt = 0;
    cmajflt = 0;
    utime = 0;
    stime = 0;
    cutime = 0;
    cstime = 0;
    priority = 0;
    nice = 0;
    num_threads = 0;
    itrealvalue = 0;
    starttime = 0;
    vsize = 0;
    rss = 0;
    rsslim = 0;
    startcode = 0;
    endcode = 0;
    startstack = 0;
    kstkesp = 0;
    kstkeip = 0;
    signal = 0;
    blocked = 0;
    sigignore = 0;
    sigcatch = 0;
    wchan = 0;
    nswap = 0;
    cnswap = 0;
    exit_signal = 0;
    processor = 0;
    rt_priority = 0;
    policy = 0;
    delayacct_blkio_ticks = 0;
    guest_time = 0;
    cguest_time = 0;
}

SrsProcSelfStat *srs_get_self_proc_stat() {
    return &_srs_system_cpu_self_stat;
}
