//
// Created by lijinwang on 2022/4/11.
//

#ifndef SSMS_SRS_HTTP_STACK_HPP
#define SSMS_SRS_HTTP_STACK_HPP

#include <srs_core.hpp>

#include <srs_kernel_io.hpp>

#include <map>
#include <string>
#include <vector>

// For srs-librtmp, @see https://github.com/ossrs/srs/issues/213
#ifndef _WIN32
#include <sys/uio.h>
#include "srs_protocol_json.hpp"

#endif

// A Header represents the key-value pairs in an HTTP header.
class SrsHttpHeader
{
private:
    // The order in which header fields with differing field names are
    // received is not significant. However, it is "good practice" to send
    // general-header fields first, followed by request-header or response-
    // header fields, and ending with the entity-header fields.
    // @doc https://tools.ietf.org/html/rfc2616#section-4.2
    std::map<std::string, std::string> headers;
public:
    SrsHttpHeader();
    virtual ~SrsHttpHeader();
public:
    // Add adds the key, value pair to the header.
    // It appends to any existing values associated with key.
    virtual void set(std::string key, std::string value);
    // Get gets the first value associated with the given key.
    // If there are no values associated with the key, Get returns "".
    // To access multiple values of a key, access the map directly
    // with CanonicalHeaderKey.
    virtual std::string get(std::string key);
    // Delete the http header indicated by key.
    // Return the removed header field.
    virtual void del(std::string);
    // Get the count of headers.
    virtual int count();
public:
    // Dumps to a JSON object.
    virtual void dumps(SrsJsonObject* o);
public:
    // Get the content length. -1 if not set.
    virtual int64_t content_length();
    // set the content length by header "Content-Length"
    virtual void set_content_length(int64_t size);
public:
    // Get the content type. empty string if not set.
    virtual std::string content_type();
    // set the content type by header "Content-Type"
    virtual void set_content_type(std::string ct);
public:
    // write all headers to string stream.
    virtual void write(std::stringstream& ss);
};

// A ResponseWriter interface is used by an HTTP handler to
// construct an HTTP response.
// Usage 0, response with a message once:
//      ISrsHttpResponseWriter* w; // create or get response.
//      std::string msg = "Hello, HTTP!";
//      w->write((char*)msg.data(), (int)msg.length());
// Usage 1, response with specified length content, same to #0:
//      ISrsHttpResponseWriter* w; // create or get response.
//      std::string msg = "Hello, HTTP!";
//      w->header()->set_content_type("text/plain; charset=utf-8");
//      w->header()->set_content_length(msg.length());
//      w->write_header(SRS_CONSTS_HTTP_OK);
//      w->write((char*)msg.data(), (int)msg.length()); // write N times, N>0
//      w->final_request(); // optional flush.
// Usage 2, response with HTTP code only, zero content length.
//      ISrsHttpResponseWriter* w; // create or get response.
//      w->header()->set_content_length(0);
//      w->write_header(SRS_CONSTS_HTTP_OK);
//      w->final_request();
// Usage 3, response in chunked encoding.
//      ISrsHttpResponseWriter* w; // create or get response.
//      std::string msg = "Hello, HTTP!";
//      w->header()->set_content_type("application/octet-stream");
//      w->write_header(SRS_CONSTS_HTTP_OK);
//      w->write((char*)msg.data(), (int)msg.length());
//      w->write((char*)msg.data(), (int)msg.length());
//      w->write((char*)msg.data(), (int)msg.length());
//      w->write((char*)msg.data(), (int)msg.length());
//      w->final_request(); // required to end the chunked and flush.
class ISrsHttpResponseWriter
{
public:
    ISrsHttpResponseWriter();
    virtual ~ISrsHttpResponseWriter();
public:
    // When chunked mode,
    // final the request to complete the chunked encoding.
    // For no-chunked mode,
    // final to send request, for example, content-length is 0.
    virtual srs_error_t final_request() = 0;

    // Header returns the header map that will be sent by WriteHeader.
    // Changing the header after a call to WriteHeader (or Write) has
    // no effect.
    virtual SrsHttpHeader* header() = 0;

    // Write writes the data to the connection as part of an HTTP reply.
    // If WriteHeader has not yet been called, Write calls WriteHeader(http.StatusOK)
    // before writing the data.  If the Header does not contain a
    // Content-Type line, Write adds a Content-Type set to the result of passing
    // The initial 512 bytes of written data to DetectContentType.
    // @param data, the data to send. NULL to flush header only.
    virtual srs_error_t write(char* data, int size) = 0;
    // for the HTTP FLV, to writev to improve performance.
    // @see https://github.com/ossrs/srs/issues/405
    virtual srs_error_t writev(const iovec* iov, int iovcnt, ssize_t* pnwrite) = 0;

    // WriteHeader sends an HTTP response header with status code.
    // If WriteHeader is not called explicitly, the first call to Write
    // will trigger an implicit WriteHeader(http.StatusOK).
    // Thus explicit calls to WriteHeader are mainly used to
    // send error codes.
    // @remark, user must set header then write or write_header.
    virtual void write_header(int code) = 0;
};

// The reader interface for http response.
class ISrsHttpResponseReader : public ISrsReader
{
public:
    ISrsHttpResponseReader();
    virtual ~ISrsHttpResponseReader();
public:
    // Whether response read EOF.
    virtual bool eof() = 0;
};


// A Request represents an HTTP request received by a server
// or to be sent by a client.
//
// The field semantics differ slightly between client and server
// usage. In addition to the notes on the fields below, see the
// documentation for Request.Write and RoundTripper.
//
// There are some modes to determine the length of body:
//      1. content-length and chunked.
//      2. infinite chunked.
//      3. no body.
// For example:
//      ISrsHttpMessage* r = ...;
//      while (!r->eof()) r->read(); // Read in mode 1 or 3.
// @rmark for mode 2, the infinite chunked, all left data is body.
class ISrsHttpMessage
{
public:
    ISrsHttpMessage();
    virtual ~ISrsHttpMessage();
public:
    virtual uint8_t method() = 0;
    virtual uint16_t status_code() = 0;
    // Method helpers.
    virtual std::string method_str() = 0;
    virtual bool is_http_get() = 0;
    virtual bool is_http_put() = 0;
    virtual bool is_http_post() = 0;
    virtual bool is_http_delete() = 0;
    virtual bool is_http_options() = 0;
public:
    // Whether should keep the connection alive.
    virtual bool is_keep_alive() = 0;
    // The uri contains the host and path.
    virtual std::string uri() = 0;
    // The url maybe the path.
    virtual std::string url() = 0;
    virtual std::string host() = 0;
    virtual std::string path() = 0;
    virtual std::string query() = 0;
    virtual std::string ext() = 0;
    // Get the RESTful id, in string,
    // for example, pattern is /api/v1/streams, path is /api/v1/streams/100,
    // then the rest id is 100.
    // @param pattern the handler pattern which will serve the request.
    // @return the REST id; "" if not matched.
    virtual std::string parse_rest_id(std::string pattern) = 0;
public:
    // Read body to string.
    // @remark for small http body.
    virtual srs_error_t body_read_all(std::string& body) = 0;
    // Get the body reader, to read one by one.
    // @remark when body is very large, or chunked, use this.
    virtual ISrsHttpResponseReader* body_reader() = 0;
    // The content length, -1 for chunked or not set.
    virtual int64_t content_length() = 0;
public:
    // Get the param in query string,
    // for instance, query is "start=100&end=200",
    // then query_get("start") is "100", and query_get("end") is "200"
    virtual std::string query_get(std::string key) = 0;
    // Get the headers.
    virtual SrsHttpHeader* header() = 0;
public:
    // Whether the current request is JSONP,
    // which has a "callback=xxx" in QueryString.
    virtual bool is_jsonp() = 0;
};

// The server mux, all http server should implements it.
class ISrsHttpServeMux
{
public:
    ISrsHttpServeMux();
    virtual ~ISrsHttpServeMux();
public:
    virtual srs_error_t serve_http(ISrsHttpResponseWriter* w, ISrsHttpMessage* r) = 0;
};

// ServeMux is an HTTP request multiplexer.
// It matches the URL of each incoming request against a list of registered
// patterns and calls the handler for the pattern that
// most closely matches the URL.
//
// Patterns name fixed, rooted paths, like "/favicon.ico",
// or rooted subtrees, like "/images/" (note the trailing slash).
// Longer patterns take precedence over shorter ones, so that
// if there are handlers registered for both "/images/"
// and "/images/thumbnails/", the latter handler will be
// called for paths beginning "/images/thumbnails/" and the
// former will receive requests for any other paths in the
// "/images/" subtree.
//
// Note that since a pattern ending in a slash names a rooted subtree,
// The pattern "/" matches all paths not matched by other registered
// patterns, not just the URL with Path == "/".
//
// Patterns may optionally begin with a host name, restricting matches to
// URLs on that host only.  Host-specific patterns take precedence over
// general patterns, so that a handler might register for the two patterns
// "/codesearch" and "codesearch.google.com/" without also taking over
// requests for "http://www.google.com/".
//
// ServeMux also takes care of sanitizing the URL request path,
// redirecting any request containing . or .. elements to an
// equivalent .- and ..-free URL.
class SrsHttpServeMux : public ISrsHttpServeMux
{
private:
    // The pattern handler, to handle the http request.
    std::map<std::string, SrsHttpMuxEntry*> entries;
    // The vhost handler.
    // When find the handler to process the request,
    // append the matched vhost when pattern not starts with /,
    // For example, for pattern /live/livestream.flv of vhost ossrs.net,
    // The path will rewrite to ossrs.net/live/livestream.flv
    std::map<std::string, ISrsHttpHandler*> vhosts;
    // all hijackers for http match.
    // For example, the hstrs(http stream trigger rtmp source)
    // can hijack and install handler when request incoming and no handler.
    std::vector<ISrsHttpMatchHijacker*> hijackers;
public:
    SrsHttpServeMux();
    virtual ~SrsHttpServeMux();
public:
    // Initialize the http serve mux.
    virtual srs_error_t initialize();
    // hijack the http match.
    virtual void hijack(ISrsHttpMatchHijacker* h);
    virtual void unhijack(ISrsHttpMatchHijacker* h);
public:
    // Handle registers the handler for the given pattern.
    // If a handler already exists for pattern, Handle panics.
    virtual srs_error_t handle(std::string pattern, ISrsHttpHandler* handler);
// Interface ISrsHttpServeMux
public:
    virtual srs_error_t serve_http(ISrsHttpResponseWriter* w, ISrsHttpMessage* r);
public:
    virtual srs_error_t find_handler(ISrsHttpMessage* r, ISrsHttpHandler** ph);
private:
    virtual srs_error_t match(ISrsHttpMessage* r, ISrsHttpHandler** ph);
    virtual bool path_match(std::string pattern, std::string path);
};

#endif //SSMS_SRS_HTTP_STACK_HPP
