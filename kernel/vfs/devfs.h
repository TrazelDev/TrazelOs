#pragma once
#include <kernel/include/vfs.h>

void init_devfs();
struct vfs_file* devfs_open(const char* filepath);
