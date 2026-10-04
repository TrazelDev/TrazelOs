#include <include/io.h>
#include <include/types.h>
#include <kernel/include/printk.h>

#include "kernel/include/cmos.h"

#define SEC_PER_MIN 60
#define SEC_PER_HOUR 3600
#define SEC_PER_DAY 86400
#define MOS_PER_YEAR 12
#define EPOCH_YEAR 1970
#define IS_LEAP_YEAR(year) ((((year) % 4 == 0) && ((year) % 100 != 0)) || ((year) % 400 == 0))
#define BCD_TO_BIN(val) (((val) & 0x0F) + (((val) >> 4) * 10))

static const int DAYS_PER_MONTH[2][MOS_PER_YEAR] = {
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};
static const int DAYS_PER_YEAR[2] = {365, 366};

enum cmos_register {
	CR_SECONDS_CMOS_REGISTER = 0x00,
	CR_MINUTES_CMOS_REGISTER = 0x02,
	CR_HOURS_CMOS_REGISTER = 0x04,
	CR_DAY_OF_WEEK_CMOS_REGISTER = 0x06,
	CR_DAY_OF_MONTH_CMOS_REGISTER = 0x07,
	CR_MONTH_CMOS_REGISTER = 0x08,
	CR_YEAR_CMOS_REGISTER = 0x09,
	CR_STATUS_A_CMOS_REGISTER = 0x0A,
	CR_STATUS_B_CMOS_REGISTER = 0x0B
};

static uint64_t g_boot_unix_epoch_seconds = 0;

static uint8_t read_cmos_register(enum cmos_register reg);
uint32_t rtc_to_unix_epoch_seconds(uint8_t seconds, uint8_t mins, uint8_t hours, uint8_t day,
								   uint8_t month, uint16_t year);
uint64_t rtc_get_unix_epoch_seconds();

void init_cmos() {
	if (g_boot_unix_epoch_seconds) {
		return;
	}

	g_boot_unix_epoch_seconds = rtc_get_unix_epoch_seconds();
	printk("alized CMOS RTC, boot unix epoch seconds: %d\n", g_boot_unix_epoch_seconds);
}
uint64_t get_boot_unix_epoch_secs() { return g_boot_unix_epoch_seconds; }

// module private functions:
// -------------------------------------------------------------------------------------------------

static uint8_t read_cmos_register(enum cmos_register reg) {
	uint8_t nmi_disable = 1 << 7;

	outb(IO_CMOS_ADDRESS_PORT, reg | nmi_disable);
	uint8_t data = inb(IO_CMOS_DATA_PORT);
	outb(IO_CMOS_ADDRESS_PORT, 0);	// re-enabling the nmi of the system

	return data;
}

uint32_t rtc_to_unix_epoch_seconds(uint8_t seconds, uint8_t mins, uint8_t hours, uint8_t day,
								   uint8_t month, uint16_t year) {
	uint32_t unix_epoch_seconds = 0;

	// Adding up all the seconds from previous years:
	uint8_t years = 0;
	uint8_t leap_years = 0;
	for (uint16_t y_k = EPOCH_YEAR; y_k < year; y_k++) {
		if (IS_LEAP_YEAR(y_k)) {
			leap_years++;
		} else {
			years++;
		}
	}
	unix_epoch_seconds +=
		((years * DAYS_PER_YEAR[0]) + (leap_years * DAYS_PER_YEAR[1])) * SEC_PER_DAY;

	// Adding up all the seconds from days in this year:
	uint8_t year_index = (IS_LEAP_YEAR(year)) ? 1 : 0;
	for (uint8_t mo_k = 0; mo_k < (month - 1); mo_k++) {  //  days from previous months this year
		unix_epoch_seconds += DAYS_PER_MONTH[year_index][mo_k] * SEC_PER_DAY;
	}
	unix_epoch_seconds += (day - 1) * SEC_PER_DAY;	// days from this month

	// Calculating seconds from today:
	unix_epoch_seconds += hours * SEC_PER_HOUR;
	unix_epoch_seconds += mins * SEC_PER_MIN;
	unix_epoch_seconds += seconds;

	return unix_epoch_seconds;
}

uint64_t rtc_get_unix_epoch_seconds() {
	uint8_t seconds = BCD_TO_BIN(read_cmos_register(CR_SECONDS_CMOS_REGISTER));
	uint8_t mins = BCD_TO_BIN(read_cmos_register(CR_MINUTES_CMOS_REGISTER));
	uint8_t hours = BCD_TO_BIN(read_cmos_register(CR_HOURS_CMOS_REGISTER));
	uint8_t day = BCD_TO_BIN(read_cmos_register(CR_DAY_OF_MONTH_CMOS_REGISTER));
	uint8_t month = BCD_TO_BIN(read_cmos_register(CR_MONTH_CMOS_REGISTER));
	uint16_t year = BCD_TO_BIN(read_cmos_register(CR_YEAR_CMOS_REGISTER)) + 2000;

	return rtc_to_unix_epoch_seconds(seconds, mins, hours, day, month, year);
}
