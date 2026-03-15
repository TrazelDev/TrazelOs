#pragma once
#include "block_device.h"

#define SECTOR_SIZE 512
struct block_device* ata_pio_init();
