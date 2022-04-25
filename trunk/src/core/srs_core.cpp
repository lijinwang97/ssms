//
// Created by lijinwang on 2022/4/1.
//

#include "srs_core.hpp"

_SrsContextId::_SrsContextId() {

}

_SrsContextId::~_SrsContextId() {

}

const char *_SrsContextId::c_str() const {
    return v_.c_str();
}

bool _SrsContextId::empty() const {
    return v_.empty();
}
