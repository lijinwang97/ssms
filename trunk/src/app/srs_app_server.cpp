//
// Created by lijinwang on 2022/4/11.
//

#include <unistd.h>
#include <srs_kernel_log.hpp>
#include "srs_app_server.hpp"

ISrsServerCycle::ISrsServerCycle() {

}

ISrsServerCycle::~ISrsServerCycle() {

}

SrsServer::SrsServer() {
    signal_reload = false;
    signal_persistence_config = false;
    signal_gmc_stop = false;
    signal_fast_quit = false;
    signal_gracefully_quit = false;
    pid_fd = -1;

    signal_manager = new SrsSignalManager(this);
    conn_manager = new SrsResourceManager("TCP", true);
    latest_version_ = new SrsLatestVersion();

    handler = NULL;
    ppid = ::getppid();

    // donot new object in constructor,
    // for some global instance is not ready now,
    // new these objects in initialize instead.
    http_api_mux = new SrsHttpServeMux();
    http_server = new SrsHttpServer(this);
    http_heartbeat = new SrsHttpHeartbeat();
    ingester = new SrsIngester();
    trd_ = new SrsSTCoroutine("srs", this, _srs_context->get_id());
    timer_ = NULL;
    wg_ = NULL;

}
