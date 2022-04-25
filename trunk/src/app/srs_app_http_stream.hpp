//
// Created by lijinwang on 2022/4/11.
//

#ifndef SSMS_SRS_APP_HTTP_STREAM_HPP
#define SSMS_SRS_APP_HTTP_STREAM_HPP

#include <srs_core.hpp>

#include <srs_app_http_conn.hpp>

class SrsAacTransmuxer;
class SrsMp3Transmuxer;
class SrsFlvTransmuxer;
class SrsTsTransmuxer;

// The HTTP Live Streaming Server, to serve FLV/TS/MP3/AAC stream.
// TODO: Support multiple stream.
class SrsHttpStreamServer : public ISrsReloadHandler
        , public ISrsHttpMatchHijacker
{
private:
    SrsServer* server;
public:
    SrsHttpServeMux mux;
    // The http live streaming template, to create streams.
    std::map<std::string, SrsLiveEntry*> tflvs;
    // The http live streaming streams, crote by template.
    std::map<std::string, SrsLiveEntry*> sflvs;
public:
    SrsHttpStreamServer(SrsServer* svr);
    virtual ~SrsHttpStreamServer();
public:
    virtual srs_error_t initialize();
public:
    // HTTP flv/ts/mp3/aac stream
    virtual srs_error_t http_mount(SrsLiveSource* s, SrsRequest* r);
    virtual void http_unmount(SrsLiveSource* s, SrsRequest* r);
// Interface ISrsReloadHandler.
public:
    virtual srs_error_t on_reload_vhost_added(std::string vhost);
    virtual srs_error_t on_reload_vhost_http_remux_updated(std::string vhost);
// Interface ISrsHttpMatchHijacker
public:
    virtual srs_error_t hijack(ISrsHttpMessage* request, ISrsHttpHandler** ph);
private:
    virtual srs_error_t initialize_flv_streaming();
    virtual srs_error_t initialize_flv_entry(std::string vhost);
};

#endif //SSMS_SRS_APP_HTTP_STREAM_HPP
