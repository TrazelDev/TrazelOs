#pragma once
#include <include/types.h>

#define VFS_FILE_NAME_LEN 100
struct vfs_file {
	char name[VFS_FILE_NAME_LEN];
	uint64_t file_size;
	uint64_t file_position;
	uint64_t file_ref_count;

	int64_t (*read)(struct vfs_file* vfs_node, uint8_t* buffer, uint64_t size);
	int64_t (*write)(struct vfs_file* vfs_node, const uint8_t* buffer, uint64_t size);
	int64_t (*close)(struct vfs_file* vfs_node);

	void* fs_private_data;
};

enum vfs_seek_whence {
	SKW_VFS_SEEK_SET = 0,  // absolute position
	SKW_VFS_SEEK_CUR = 1,  // relative to the current position
	SKW_VFS_SEEK_END = 2   // relative to the end
};

void vfs_init();
struct vfs_file* vfs_open(const char* filename);
int64_t vfs_read(struct vfs_file* vfs_node, uint8_t* buffer, uint64_t size);
int64_t vfs_close(struct vfs_file* vfs_node);
int64_t vfs_seek(struct vfs_file* vfs_node, int64_t offset, enum vfs_seek_whence whence);
