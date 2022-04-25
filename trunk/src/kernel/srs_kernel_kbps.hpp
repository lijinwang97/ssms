#ifndef SSMS_SRS_KERNEL_KBPS_HPP
#define SSMS_SRS_KERNEL_KBPS_HPP

#include <cstdint>

class SrsWallClock;

// A sample for rate-based stat, such as kbps or kps.
class SrsRateSample
{
public:
    int64_t total;
    srs_utime_t time;
    // kbps or kps
    int rate;
public:
    SrsRateSample();
    virtual ~SrsRateSample();
public:
    virtual SrsRateSample* update(int64_t nn, srs_utime_t t, int k);
};

// A pps manager every some duration. 每隔一段时间就有一个pps经理。
class SrsPps{
private:
    SrsWallClock* clk_;
private:
    // samples
    SrsRateSample sample_10s_;
    SrsRateSample sample_30s_;
    SrsRateSample sample_1m_;
    SrsRateSample sample_5m_;
    SrsRateSample sample_60m_;

public:
    // Sugar for target to stat.
    int64_t sugar;
public:
    SrsPps();
    virtual ~SrsPps();
public:
    // Update with the nn which is target.
    void update();
    // Update with the nn.
    void update(int64_t nn);
    // Get the 10s average stat.
    int r10s();

};

class SrsWallClock{
public:
    SrsWallClock();
    virtual ~SrsWallClock();

public:
    /**
 * Current time in srs_utime_t.
 */
    virtual srs_utime_t now();
};

extern SrsWallClock* _srs_clock;

#endif //SSMS_SRS_KERNEL_KBPS_HPP