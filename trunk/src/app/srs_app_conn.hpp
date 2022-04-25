#ifndef SSMS_SRS_APP_CONN_HPP
#define SSMS_SRS_APP_CONN_HPP

#include <srs_kernel_kbps.hpp>
#include <stddef.h>

#include <srs_core.hpp>

#include <string>
#include <vector>
#include <map>

#include <openssl/ssl.h>

#include <srs_app_st.hpp>
#include <srs_app_reload.hpp>
#include <srs_service_conn.hpp>

SrsPps* _srs_pps_dispose = NULL;

// The resource manager remove resource and delete it asynchronously.
class SrsResourceManager : public ISrsCoroutineHandler, public ISrsResourceManager
{
private:
    std::string label_;
    SrsContextId cid_;
    bool verbose_;
private:
    SrsCoroutine* trd;
    srs_cond_t cond;
    // Callback handlers.
    std::vector<ISrsDisposingHandler*> handlers_;
    // Unsubscribing handlers, skip it for notifying.
    std::vector<ISrsDisposingHandler*> unsubs_;
    // Whether we are removing resources.
    bool removing_;
    // The zombie connections, we will delete it asynchronously.
    std::vector<ISrsResource*> zombies_;
    std::vector<ISrsResource*>* p_disposing_;
private:
    // The connections without any id.
    std::vector<ISrsResource*> conns_;
    // The connections with resource id.
    std::map<std::string, ISrsResource*> conns_id_;
    // The connections with resource fast(int) id.
    std::map<uint64_t, ISrsResource*> conns_fast_id_;
    // The level-0 fast cache for fast id.
    int nn_level0_cache_;
    SrsResourceFastIdItem* conns_level0_cache_;
    // The connections with resource name.
    std::map<std::string, ISrsResource*> conns_name_;
public:
    SrsResourceManager(const std::string& label, bool verbose = false);
    virtual ~SrsResourceManager();
public:
    srs_error_t start();
    bool empty();
    size_t size();
// Interface ISrsCoroutineHandler
public:
    virtual srs_error_t cycle();
public:
    void add(ISrsResource* conn, bool* exists = NULL);
    void add_with_id(const std::string& id, ISrsResource* conn);
    void add_with_fast_id(uint64_t id, ISrsResource* conn);
    void add_with_name(const std::string& name, ISrsResource* conn);
    ISrsResource* at(int index);
    ISrsResource* find_by_id(std::string id);
    ISrsResource* find_by_fast_id(uint64_t id);
    ISrsResource* find_by_name(std::string name);
public:
    void subscribe(ISrsDisposingHandler* h);
    void unsubscribe(ISrsDisposingHandler* h);
// Interface ISrsResourceManager
public:
    virtual void remove(ISrsResource* c);
private:
    void do_remove(ISrsResource* c);
    void check_remove(ISrsResource* c, bool& in_zombie, bool& in_disposing);
    void clear();
    void do_clear();
    void dispose(ISrsResource* c);
};

#endif //SSMS_SRS_APP_CONN_HPP
