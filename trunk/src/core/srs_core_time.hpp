#ifndef SSMS_SRS_CORE_TIME_HPP
#define SSMS_SRS_CORE_TIME_HPP

#include <cstdint>

// Time and duration unit, in us.
typedef int64_t srs_utime_t;

// The time unit in ms, for example 100 * SRS_UTIME_MILLISECONDS means 100ms.
#define SRS_UTIME_MILLISECONDS 1000

// The time unit in ms, for example 120 * SRS_UTIME_SECONDS means 120s.
#define SRS_UTIME_SECONDS 1000000LL

// Convert srs_utime_t as ms.
#define srsu2ms(us) ((us) / SRS_UTIME_MILLISECONDS)
#define srsu2msi(us) int((us) / SRS_UTIME_MILLISECONDS)

#endif //SSMS_SRS_CORE_TIME_HPP
