#pragma once
#include <drivers/block_device.h>
#include <kernel/include/vfs.h>

void init_fat12(struct block_device* blk_dev);
struct vfs_file* fat12_open(const char* filepath);
