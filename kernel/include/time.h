#pragma once
#include <include/types.h>

typedef int64_t time_t;
typedef int64_t suseconds_t;

struct timeval {
	time_t tv_sec;
	suseconds_t tv_usec;
};

void time_pit_spin_sleep_ms(uint32_t milliseconds);
int gettimeofday(struct timeval* timeval);
