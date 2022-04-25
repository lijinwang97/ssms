#ifndef SSMS_SRS_CORE_H
#define SSMS_SRS_CORE_H

#include <stddef.h>

// To free the p and set to NULL.
// @remark The p must be a pointer T*.
#define srs_freep(p) \
    if (p) {         \
        delete p;    \
        p = NULL; \
    } \
    (void)0

// Please use the freepa(T[]) to free an array, otherwise the behavior is undefined.
#define srs_freepa(pa) \
    if (pa) {         \
        delete[] pa;    \
        pa = NULL; \
    } \
    (void)0

#include <assert.h>
#ifndef srs_assert
#define srs_assert(expression) assert(expression)
#endif

class SrsCplxError;

typedef SrsCplxError *srs_error_t;

#include <string>

class _SrsContextId {
private:
    std::string v_;
public:
    _SrsContextId();

    virtual ~_SrsContextId();

public:
    const char *c_str() const;
    bool empty() const;
};

typedef _SrsContextId SrsContextId;


#endif //SSMS_SRS_CORE_H
