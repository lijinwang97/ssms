#ifndef SSMS_SRS_APP_CONFIG_H
#define SSMS_SRS_APP_CONFIG_H

#include <string>
#include <vector>
#include <srs_app_reload.hpp>

class SrsConfDirective {
public:
    // The line of config file in which the directive from
    int conf_line; // 指令所在的配置文件行

    // The name of directive, for example, the following config text:
    //       enabled     on;
    // will be parsed to a directive, its name is "enalbed"
    std::string name; // 指令名称
    // The args of directive, for example, the following config text:
    //       listen      1935 1936;
    // will be parsed to a directive, its args is ["1935", "1936"].
    std::vector<std::string> args;
    // The child directives, for example, the following config text:
    //       vhost vhost.ossrs.net {
    //           enabled         on;
    //       }
    // will be parsed to a directive, its directives is a vector contains
    // a directive, which is:
    //       name:"enalbed", args:["on"], directives:[]
    //
    // @remark, the directives can contains directives.
    std::vector<SrsConfDirective *> directives; //子指令
public:
    SrsConfDirective();

    virtual ~SrsConfDirective();

public:
    virtual SrsConfDirective* get(std::string _name);

public:
    // Get the args0,1,2, if user want to get more args,
    // directly use the args.at(index). 获取args0,1,2，如果用户想要获得更多的args，直接使用args.at(index)。
    virtual std::string arg0();
    virtual std::string arg1();
    virtual std::string arg2();


};

class SrsConfig {
private:
    // Whether srs is run in dolphin mode.
    // @see https://github.com/ossrs/srs-dolphin
    bool dolphin;

    // Whether show help and exit.
    bool show_help;
    // Whether show SRS version and exit.
    bool show_version;
    // Whether test config file and exit.
    bool test_config;
    // Whether show SRS signature and exit.
    bool show_signature;

protected:
    SrsConfDirective *root;
private:
    // The reload subscribers, when reload, callback all handlers.重新加载订阅者，当重新加载时，回调所有处理程序。
    std::vector<ISrsReloadHandler *> subscribes;
public:
    SrsConfig();

    virtual ~SrsConfig();

public:
    // For reload handler to unregister itself. 重新加载处理程序取消自身注册。
    virtual void unsubscribe(ISrsReloadHandler *handler);

// log section
public:
    virtual std::string get_log_file();


};


extern SrsConfig *_srs_config;


#endif //SSMS_SRS_APP_CONFIG_H
