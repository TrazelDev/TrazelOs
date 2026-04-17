#include "include/io.h"
#include "pit.h"

#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL_2_PORT 0x42
#define PC_SPEAKER_PORT 0x61
#define PIT_TICKS_PER_MS 1193

void pit_spin_sleep_ms(uint32_t ms) {
	for (uint32_t i = 0; i < ms; i++) {
		outb(PIT_COMMAND_PORT, 0xB0);

		outb(PIT_CHANNEL_2_PORT, PIT_TICKS_PER_MS & 0xFF);
		outb(PIT_CHANNEL_2_PORT, (PIT_TICKS_PER_MS >> 8) & 0xFF);

		uint8_t val = inb(PC_SPEAKER_PORT);
		outb(PC_SPEAKER_PORT, val | 0x01);

		while ((inb(PC_SPEAKER_PORT) & 0x20) == 0) {
			asm volatile("pause");
		}

		outb(PC_SPEAKER_PORT, val & ~0x01);
	}
}
