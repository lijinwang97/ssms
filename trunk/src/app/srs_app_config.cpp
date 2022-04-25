#include <srs_core.hpp>
#include "srs_app_config.hpp"

SrsConfig::SrsConfig() {
    dolphin = false;

    show_help = false;
    show_version = false;
    test_config = false;
    show_signature = false;

    root = new SrsConfDirective();
    root->conf_line = 0;
    root->name = "root";
}

SrsConfig::~SrsConfig() {
    srs_freep(root);
}

void SrsConfig::unsubscribe(ISrsReloadHandler *handler) {
    std::vector<ISrsReloadHandler *>::iterator it;
    it = std::find(subscribes.begin(), subscribes.end(), handler);
    if (it == subscribes.end()) {
        return;
    }
    subscribes.erase(it);
}

std::string SrsConfig::get_log_file() {
    static std::string DEFAULT = "./objs/srs.log";
    SrsConfDirective *conf = root->get("srs_log_file");
    if (!conf || conf->arg0().empty()) {
        return DEFAULT;
    }
    return conf->arg0();
}

SrsConfDirective::SrsConfDirective() {
    conf_line = 0;

}

SrsConfDirective::~SrsConfDirective() {
    std::vector<SrsConfDirective *>::iterator it;
    for (it = directives.begin(); it != directives.end(); it++) {
        SrsConfDirective *directive = *it;
        srs_freep(directive);
    }
    directives.clear();
}

SrsConfDirective *SrsConfDirective::get(std::string _name) {
    std::vector<SrsConfDirective *>::iterator it;
    for (it = directives.begin(); it != directives.end(); ++it) {
        SrsConfDirective *directive = *it;
        if (directive->name == _name) {
            return directive;
        }
    }
    return NULL;
}

std::string SrsConfDirective::arg0() {
    if (args.size() > 0) {
        return args.at(0);
    }
    return "";
}

std::string SrsConfDirective::arg1() {
    if (args.size() > 1) {
        return args.at(1);
    }
    return "";
}

std::string SrsConfDirective::arg2() {
    if (args.size() > 2) {
        return args.at(2);
    }
    return "";
}
