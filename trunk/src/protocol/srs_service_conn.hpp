//
// Created by lijinwang on 2022/4/11.
//

#ifndef SSMS_SRS_SERVICE_CONN_HPP
#define SSMS_SRS_SERVICE_CONN_HPP

#include <srs_core.hpp>

#include <string>

// The resource managed by ISrsResourceManager.
class ISrsResource
{
public:
    ISrsResource();
    virtual ~ISrsResource();
public:
    // Get the context id of connection.
    virtual const SrsContextId& get_id() = 0;
    // The resource description, optional.
    virtual std::string desc() = 0;
};

// The manager for resource. 资源管理器。
class ISrsResourceManager
{
public:
    ISrsResourceManager();
    virtual ~ISrsResourceManager();
public:
    // Remove then free the specified connection. 移除并释放指定的连接。
    virtual void remove(ISrsResource* c) = 0;
};

#endif //SSMS_SRS_SERVICE_CONN_HPP
