#pragma once
#include <include/types.h>

#define VFS_FILE_NAME_LEN 256
struct vfs_file {
	char name[VFS_FILE_NAME_LEN];
	uint32_t file_size;
	uint32_t file_position;

	int (*read)(struct vfs_file* vfs_node, uint8_t* buffer, uint32_t size);
	int (*close)(struct vfs_file* vfs_node);

	void* fs_private_data;
};

void vfs_init();
struct vfs_file* vfs_open(const char* filename);
