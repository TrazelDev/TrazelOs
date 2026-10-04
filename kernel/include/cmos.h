#pragma once
#include <include/types.h>

void init_cmos();

/** Returns the unix epoch time from 1970 to time the os booted */
uint64_t cmos_get_boot_unix_epoch_secs();
