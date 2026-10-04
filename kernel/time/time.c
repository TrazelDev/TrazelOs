#include "kernel/include/apic.h"
#include "kernel/include/cmos.h"
#include "kernel/include/time.h"

#define MILLISECONDS_PER_SECOND 1000
#define MICROSECONDS_PER_MILLISECOND 1000

int gettimeofday(struct timeval* timeval) {
	uint64_t boot_unix_epoch_seconds = cmos_get_boot_unix_epoch_secs();
	uint64_t milliseconds_since_boot = apic_get_system_milliseconds_uptime();

	timeval->tv_sec =
		(time_t)(boot_unix_epoch_seconds + (milliseconds_since_boot / MILLISECONDS_PER_SECOND));
	timeval->tv_usec = (suseconds_t)((milliseconds_since_boot % MILLISECONDS_PER_SECOND) *
									 MICROSECONDS_PER_MILLISECOND);

	return 0;
}
