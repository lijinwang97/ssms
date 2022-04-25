//
// Created by lijinwang on 2022/4/10.
//

#ifndef SSMS_SRS_APP_SOURCE_HPP
#define SSMS_SRS_APP_SOURCE_HPP
#include <srs_app_hourglass.hpp>

#include <map>
#include <vector>
#include <string>
#include "srs_app_reload.hpp"

class SrsFormat;
class SrsRtmpFormat;
class SrsLiveConsumer;
class SrsPlayEdge;
class SrsPublishEdge;
class SrsLiveSource;
class SrsCommonMessage;
class SrsOnMetaDataPacket;
class SrsSharedPtrMessage;
class SrsForwarder;
class SrsRequest;
class SrsStSocket;
class SrsRtmpServer;
class SrsEdgeProxyContext;
class SrsMessageArray;
class SrsNgExec;
class SrsMessageHeader;
class SrsHls;
class SrsRtc;
class SrsDvr;
class SrsDash;
class SrsEncoder;
class SrsBuffer;

// The handler to handle the event of srs source.
// For example, the http flv streaming module handle the event and
// mount http when rtmp start publishing.
class ISrsLiveSourceHandler
{
public:
    ISrsLiveSourceHandler();
    virtual ~ISrsLiveSourceHandler();
public:
    // when stream start publish, mount stream.
    virtual srs_error_t on_publish(SrsLiveSource* s, SrsRequest* r) = 0;
    // when stream stop publish, unmount stream.
    virtual void on_unpublish(SrsLiveSource* s, SrsRequest* r) = 0;
};

// The live streaming source.
class SrsLiveSource : public ISrsReloadHandler
{
    friend class SrsOriginHub;
private:
    // For publish, it's the publish client id. 对于发布，它是发布客户端id。
    // For edge, it's the edge ingest id.
    // when source id changed, for example, the edge reconnect,
    // invoke the on_source_id_changed() to let all clients know.
    SrsContextId _source_id;
    // previous source id.
    SrsContextId _pre_source_id;
    // deep copy of client request.
    SrsRequest* req;
    // To delivery stream to clients.
    std::vector<SrsLiveConsumer*> consumers;
    // The time jitter algorithm for vhost.
    SrsRtmpJitterAlgorithm jitter_algorithm;
    // For play, whether use interlaced/mixed algorithm to correct timestamp.
    bool mix_correct;
    // The mix queue to implements the mix correct algorithm.
    SrsMixQueue* mix_queue;
    // For play, whether enabled atc.
    // The atc(use absolute time and donot adjust time),
    // directly use msg time and donot adjust if atc is true,
    // otherwise, adjust msg time to start from 0 to make flash happy.
    bool atc;
    // whether stream is monotonically increase.
    bool is_monotonically_increase;
    // The time of the packet we just got.
    int64_t last_packet_time;
    // The event handler.
    ISrsLiveSourceHandler* handler;
    // The source bridger for other source.
    ISrsLiveSourceBridger* bridger_;
    // The edge control service
    SrsPlayEdge* play_edge;
    SrsPublishEdge* publish_edge;
    // The gop cache for client fast startup.
    SrsGopCache* gop_cache;
    // The hub for origin server.
    SrsOriginHub* hub;
    // The metadata cache.
    SrsMetaCache* meta;
private:
    // Whether source is avaiable for publishing.
    bool _can_publish;
    // The last die time, when all consumers quit and no publisher,
    // We will remove the source when source die.
    srs_utime_t die_at;
public:
    SrsLiveSource();
    virtual ~SrsLiveSource();
public:
    virtual void dispose();
    virtual srs_error_t cycle();
    // Remove source when expired.
    virtual bool expired();
public:
    // Initialize the hls with handlers.
    virtual srs_error_t initialize(SrsRequest* r, ISrsLiveSourceHandler* h);
    // Bridge to other source, forward packets to it.
    void set_bridger(ISrsLiveSourceBridger* v);
// Interface ISrsReloadHandler
public:
    virtual srs_error_t on_reload_vhost_play(std::string vhost);
public:
    // The source id changed.
    virtual srs_error_t on_source_id_changed(SrsContextId id);
    // Get current source id.
    virtual SrsContextId source_id();
    virtual SrsContextId pre_source_id();
    // Whether source is inactive, which means there is no publishing stream source.
    // @remark For edge, it's inactive util stream has been pulled from origin.
    virtual bool inactive();
    // Update the authentication information in request.
    virtual void update_auth(SrsRequest* r);
public:
    virtual bool can_publish(bool is_edge);
    virtual srs_error_t on_meta_data(SrsCommonMessage* msg, SrsOnMetaDataPacket* metadata);
public:
    // TODO: FIXME: Use SrsSharedPtrMessage instead.
    virtual srs_error_t on_audio(SrsCommonMessage* audio);
private:
    virtual srs_error_t on_audio_imp(SrsSharedPtrMessage* audio);
public:
    // TODO: FIXME: Use SrsSharedPtrMessage instead.
    virtual srs_error_t on_video(SrsCommonMessage* video);
private:
    virtual srs_error_t on_video_imp(SrsSharedPtrMessage* video);
public:
    virtual srs_error_t on_aggregate(SrsCommonMessage* msg);
    // Publish stream event notify.
    // @param _req the request from client, the source will deep copy it,
    //         for when reload the request of client maybe invalid.
    virtual srs_error_t on_publish();
    virtual void on_unpublish();
public:
    // Create consumer
    // @param consumer, output the create consumer.
    virtual srs_error_t create_consumer(SrsLiveConsumer*& consumer);
    // Dumps packets in cache to consumer.
    // @param ds, whether dumps the sequence header.
    // @param dm, whether dumps the metadata.
    // @param dg, whether dumps the gop cache.
    virtual srs_error_t consumer_dumps(SrsLiveConsumer* consumer, bool ds = true, bool dm = true, bool dg = true);
    virtual void on_consumer_destroy(SrsLiveConsumer* consumer);
    virtual void set_cache(bool enabled);
    virtual SrsRtmpJitterAlgorithm jitter();
public:
    // For edge, when publish edge stream, check the state
    virtual srs_error_t on_edge_start_publish();
    // For edge, proxy the publish
    virtual srs_error_t on_edge_proxy_publish(SrsCommonMessage* msg);
    // For edge, proxy stop publish
    virtual void on_edge_proxy_unpublish();
public:
    virtual std::string get_curr_origin();
};

// The source manager to create and refresh all stream sources. 创建和刷新所有流源的源管理器。
class SrsLiveSourceManager : public ISrsHourGlass
{
private:
    srs_mutex_t lock;
    std::map<std::string, SrsLiveSource*> pool;
    SrsHourGlass* timer_;
public:
    SrsLiveSourceManager();
    virtual ~SrsLiveSourceManager();
public:
    virtual srs_error_t initialize();
    //  create source when fetch from cache failed.
    // @param r the client request.
    // @param h the event handler for source.
    // @param pps the matched source, if success never be NULL.
    virtual srs_error_t fetch_or_create(SrsRequest* r, ISrsLiveSourceHandler* h, SrsLiveSource** pps);
public:
    // Get the exists source, NULL when not exists.
    virtual SrsLiveSource* fetch(SrsRequest* r);
public:
    // dispose and cycle all sources.
    virtual void dispose();
// interface ISrsHourGlass
private:
    virtual srs_error_t setup_ticks();
    virtual srs_error_t notify(int event, srs_utime_t interval, srs_utime_t tick);
public:
    // when system exit, destroy th`e sources,
    // For gmc to analysis mem leaks.
    virtual void destroy();
};

#endif //SSMS_SRS_APP_SOURCE_HPP
