#include "clock.hpp"

#ifdef WIN32
#include <windows.h>
#else
#include <time.h>
#endif

namespace azrpc {

int64_t clock_time(void) {
#ifdef WIN32
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
	// ft now contains a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC).
	ULARGE_INTEGER temp;
	temp.HighPart = ft.dwHighDateTime;
	temp.LowPart = ft.dwLowDateTime;
    return (uint64) (temp.QuadPart / 10000);
#else
	struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC,&tv);
    return (int64_t) (tv.tv_sec * 1000 + tv.tv_nsec / 1000000);
#endif
}
}


