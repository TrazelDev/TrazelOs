#pragma once
#include <types.h>

void ata_pio_init();
void ata_pio_read(uint64_t lba_addressing, uint64_t sectors_to_read, char* buf);
void ata_pio_write(uint64_t lba_addressing, uint64_t sectors_to_write, char* buf);

/*
 * Tests that the driver is working
 * WARNING: This test breaks the bootsector, so run the os and than rebuild it
 * after so it will work
*/
bool ata_pio_min_test();
