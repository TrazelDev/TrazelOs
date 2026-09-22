#include <drivers/ata_pio.h>
#include <drivers/block_device.h>
#include <include/mem_utils.h>
#include <include/types.h>
#include <kernel/include/heap.h>
#include <kernel/include/panic.h>
#include <kernel/include/printk.h>

#include "drivers/fat12.h"
#include "fat12_defs.h"

static struct block_device* g_blk_dev;
static struct fat12_header g_fat12_header;
static struct fat12_info g_fat12_info;

static struct fat12_directory_entry* get_path_element_dir_entry(
	struct fat12_directory_entry* dir_entries, const char* path_element, uint64_t path_element_len,
	uint32_t dir_entries_count);
static uint32_t get_filtered_directory_entries(struct fat12_directory_entry** dirs,
											   struct fat12_directory_entry* dir_entry,
											   struct fat12_info* fat12_info);

void init_fat12(struct block_device* blk_dev) {
	g_blk_dev = blk_dev;

	load_fat12_header(&g_fat12_header);
	load_fat12_info(&g_fat12_info, &g_fat12_header);
	printk("Initialized fat12 driver Successfully\n");
}

int vfs_fat12_read(struct vfs_file* vfs_node, uint8_t* buffer, uint32_t size) {
	memcpy(buffer, (uint8_t*)vfs_node->fs_private_data + vfs_node->file_position, size);
	return size;
}

int vfs_fat12_close(struct vfs_file* vfs_node) {
	kfree(vfs_node->fs_private_data);
	kfree(vfs_node);
	return 0;
}

struct vfs_file* fat12_open(const char* filepath) {
	struct vfs_file* file_vfs = kmalloc(sizeof(struct vfs_file));

	struct fat12_directory_entry* dir_entries;
	struct fat12_directory_entry* dir_entries_backup;
	uint32_t dir_entries_count = get_root_directory_entries(&dir_entries, &g_fat12_info);

	const char* curr = filepath;
	if (*curr == '/') {
		curr++;
	}
	const char* curr_path_element = curr;
	uint64_t path_element_len = 0;
	while (*curr) {
		if (*curr == '/') {
			struct fat12_directory_entry* dir_entry = get_path_element_dir_entry(
				dir_entries, curr_path_element, path_element_len, dir_entries_count);
			KERNEL_ASSERT(dir_entry != NULL, "file with this path does not exist");
			dir_entries_backup = dir_entries;
			dir_entries_count =
				get_filtered_directory_entries(&dir_entries, dir_entry, &g_fat12_info);
			kfree(dir_entries_backup);

			curr++;
			curr_path_element = curr;
			path_element_len = 0;
			continue;
		}

		path_element_len++;
		curr++;
	}

	struct fat12_directory_entry* file_directory_entry = get_path_element_dir_entry(
		dir_entries, curr_path_element, path_element_len, dir_entries_count);
	KERNEL_ASSERT(file_directory_entry != NULL, "file with this path does not exist");

	file_vfs->file_size = get_file_content((uint8_t**)&file_vfs->fs_private_data,
										   file_directory_entry, &g_fat12_info);
	file_vfs->file_position = 0;
	file_vfs->read = vfs_fat12_read;
	file_vfs->close = NULL;

	kfree(dir_entries);
	return file_vfs;
}

// module private functions:
// -------------------------------------------------------------------------------------------------

static struct fat12_directory_entry* get_path_element_dir_entry(
	struct fat12_directory_entry* dir_entries, const char* path_element, uint64_t path_element_len,
	uint32_t dir_entries_count) {
	struct fat12_directory_entry* return_entry = NULL;
	char** elements_names;
	get_entries_file_names(&elements_names, dir_entries, dir_entries_count);

	for (uint64_t i = 0; i < dir_entries_count; i++) {
		if (strncmp(path_element, elements_names[i], path_element_len) == 0 &&
			elements_names[i][path_element_len] == '\0') {
			return_entry = &dir_entries[i];
			break;
		}
	}

	for (uint64_t i = 0; i < dir_entries_count; i++) {
		kfree(elements_names[i]);
	}
	kfree(elements_names);
	return return_entry;
}

static uint32_t get_filtered_directory_entries(struct fat12_directory_entry** dirs,
											   struct fat12_directory_entry* dir_entry,
											   struct fat12_info* fat12_info) {
	uint64_t directory_bytes = get_file_content((uint8_t**)dirs, dir_entry, fat12_info);
	uint32_t max_entries = directory_bytes / sizeof(struct fat12_directory_entry);

	return filter_valid_directory_entries(dirs, max_entries);
}

void pread_device(uint8_t* buffer, uint64_t read_bytes, int64_t offset) {
	if (!read_blk_device_bytes(g_blk_dev, offset, buffer, read_bytes)) {
		printk("\nFailed to read from the device\n");
	}
}

struct fat12_header* load_fat12_header(struct fat12_header* fat12_header) {
	static char buffer[sizeof(struct fat12_header)];

	pread_device((uint8_t*)buffer, sizeof(struct fat12_header), 0);
	memcpy(fat12_header, buffer, sizeof(struct fat12_header));

	return fat12_header;
}

struct fat12_info* load_fat12_info(struct fat12_info* fat12_info,
								   struct fat12_header* fat12_header) {
	struct fat12_info* info = fat12_info;
	uint32_t root_dir_bytes = fat12_header->root_entry_count * sizeof(struct fat12_directory_entry);

	if (fat12_header->total_sectors16) {
		info->total_sectors = fat12_header->total_sectors16;
	} else {
		info->total_sectors = fat12_header->total_sectors32;
	}
	info->bytes_per_sector = fat12_header->bytes_per_sector;
	info->sectors_per_cluster = fat12_header->sectors_per_cluster;
	info->fat_sector_size = fat12_header->table_size16;
	info->fat_section_sector_size = fat12_header->table_count * info->fat_sector_size;
	info->root_dir_sectors_size =
		bytes_to_sectors_round_up(root_dir_bytes, fat12_header->bytes_per_sector);
	info->data_sectors_size = info->total_sectors - fat12_header->reserved_sector_count -
							  info->fat_section_sector_size - info->root_dir_sectors_size;
	info->cluster_count = info->data_sectors_size / fat12_header->sectors_per_cluster;

	info->data_section_sector_offset = fat12_header->reserved_sector_count +
									   info->fat_section_sector_size + info->root_dir_sectors_size;
	info->fat_section_sector_offset = fat12_header->reserved_sector_count;
	info->root_dir_sector_offset = info->data_section_sector_offset - info->root_dir_sectors_size;

	return info;
}

uint32_t count_valid_entries(struct fat12_directory_entry* dir_entries, uint32_t max_entries,
							 bool include_none_file_or_dir_entries) {
	uint32_t count = 0;
	for (uint32_t i = 0; i < max_entries; i++) {
		if (is_final_directory_entry(&dir_entries[i])) {
			break;	// End of the directory
		}
		if (include_none_file_or_dir_entries) {
			count++;
			continue;
		}
		if (!is_deleted_entry(&dir_entries[i]) && !is_volume_label_entry(&dir_entries[i])) {
			count++;
		}
	}

	return count;
}

uint32_t get_directory_entries(struct fat12_directory_entry** dirs,
							   struct fat12_directory_entry* dir_entry,
							   struct fat12_info* fat12_info) {
	uint64_t directory_size = get_file_content((uint8_t**)dirs, dir_entry, fat12_info);
	return directory_size;
}

uint32_t get_entries_file_names(char*** file_names, struct fat12_directory_entry* dir_entries,
								uint32_t dir_entries_count) {
	// Directories names are included in this count:
	uint32_t file_type_entries_count = count_valid_entries(dir_entries, dir_entries_count, false);

	*file_names = (char**)kmalloc(file_type_entries_count * sizeof(char*));
	int name_index = 0;
	for (uint32_t i = 0; i < dir_entries_count; i++) {
		if (is_final_directory_entry(&dir_entries[i])) {
			break;
		}
		if (is_deleted_entry(&dir_entries[i])) {
			continue;
		}
		if (is_volume_label_entry(&dir_entries[i])) {
			continue;
		}

		char* val = fat_file_name_to_str(dir_entries[i].file_name);
		(*file_names)[name_index] = val;
		name_index++;
	}

	return file_type_entries_count;
}

uint32_t get_root_file_names(char*** names, struct fat12_info* fat12_info) {
	struct fat12_directory_entry* dir_entries;
	uint32_t entries_count = get_root_directory_entries(&dir_entries, fat12_info);

	uint32_t file_names_count = get_entries_file_names(names, dir_entries, entries_count);
	kfree(dir_entries);
	return file_names_count;
}

uint32_t filter_valid_directory_entries(struct fat12_directory_entry** dir_entries,
										uint32_t entries_count) {
	// Directories names are included in this count:
	uint32_t file_type_entries_count = count_valid_entries(*dir_entries, entries_count, false);

	struct fat12_directory_entry* filtered_entries =
		kmalloc(file_type_entries_count * sizeof(struct fat12_directory_entry));
	int valid_index = 0;
	struct fat12_directory_entry* curr_element;
	for (uint32_t i = 0; i < entries_count; i++) {
		curr_element = &(*dir_entries)[i];
		if (is_final_directory_entry(curr_element)) {
			break;
		}
		if (is_deleted_entry(curr_element)) {
			continue;
		}
		if (is_volume_label_entry(curr_element)) {
			continue;
		}

		memcpy(&filtered_entries[valid_index], curr_element, sizeof(struct fat12_directory_entry));
		valid_index++;
	}
	kfree(*dir_entries);
	*dir_entries = filtered_entries;
	return file_type_entries_count;
}

uint32_t get_file_content(uint8_t** file_content,
						  struct fat12_directory_entry* file_directory_entry,
						  struct fat12_info* fat12_info) {
	const uint32_t BYTES_PER_CLUSTER =
		fat12_info->bytes_per_sector * fat12_info->sectors_per_cluster;

	uint8_t* fat = get_fat(fat12_info);
	uint32_t file_cluster_count = count_file_clusters(file_directory_entry->first_cluster_id, fat);
	*file_content = kmalloc((uint64_t)file_cluster_count * BYTES_PER_CLUSTER);
	uint8_t* curr_file_content_ptr = *file_content;

	uint32_t curr_cluster_id = file_directory_entry->first_cluster_id;
	char* cluster_data;
	for (uint32_t i = 0; i < file_cluster_count; i++) {
		read_cluster(&cluster_data, curr_cluster_id, fat12_info);
		memcpy(curr_file_content_ptr, cluster_data, BYTES_PER_CLUSTER);
		kfree(cluster_data);

		curr_file_content_ptr += BYTES_PER_CLUSTER;
		curr_cluster_id = get_next_cluster_id(curr_cluster_id, fat);
	}

	if (is_directory_entry_directory(file_directory_entry)) {
		return BYTES_PER_CLUSTER * file_cluster_count;
	}
	return file_directory_entry->file_size_in_bytes;
}

uint8_t* get_fat(struct fat12_info* fat12_info) {
	const uint32_t FAT12_TABLE_SIZE = fat12_info->fat_sector_size * fat12_info->bytes_per_sector;
	const uint32_t FAT_BYTE_OFFSET =
		fat12_info->bytes_per_sector * fat12_info->fat_section_sector_offset;

	uint8_t* fat = kmalloc(FAT12_TABLE_SIZE);
	pread_device(fat, FAT12_TABLE_SIZE, FAT_BYTE_OFFSET);

	return fat;
}

uint16_t get_next_cluster_id(uint16_t cluster_id, const uint8_t* fat) {
	uint32_t offset = cluster_id + (cluster_id / 2);
	int packed = fat[offset] | (fat[offset + 1] << 8);

	if (cluster_id % 2) {
		return packed >> 4;
	}
	return packed & 0x0FFF;
}

uint32_t count_file_clusters(uint16_t initial_cluster_id, const uint8_t* fat) {
	uint32_t cluster_count = 0;
	uint16_t curr_cluster_id = initial_cluster_id;
	while (curr_cluster_id != FAT_LAST_CLUSTER_NUM) {
		cluster_count++;
		curr_cluster_id = get_next_cluster_id(curr_cluster_id, fat);
	}

	return cluster_count;
}

uint32_t read_cluster(char** data, uint16_t cluster_id, struct fat12_info* fat12_info) {
	uint32_t cluster_num = cluster_id_to_cluster_num(cluster_id);
	uint32_t bytes_per_cluster = fat12_info->sectors_per_cluster * fat12_info->bytes_per_sector;
	uint32_t data_section_sector_offset = cluster_num * fat12_info->sectors_per_cluster;
	uint32_t device_sector_offset =
		fat12_info->data_section_sector_offset + data_section_sector_offset;
	uint32_t device_bytes_offset = device_sector_offset * fat12_info->bytes_per_sector;

	*data = kmalloc(bytes_per_cluster);
	pread_device((uint8_t*)*data, bytes_per_cluster, device_bytes_offset);
	return bytes_per_cluster;
}
uint32_t get_root_directory_entries(struct fat12_directory_entry** dir_entries,
									struct fat12_info* fat12_info) {
	const uint32_t DIRECTORY_BYTES_SIZE =
		fat12_info->root_dir_sectors_size * fat12_info->bytes_per_sector;
	const uint32_t BYTES_OFFSET = fat12_info->root_dir_sector_offset * fat12_info->bytes_per_sector;
	const uint32_t MAX_ENTRIES = DIRECTORY_BYTES_SIZE / sizeof(struct fat12_directory_entry);

	*dir_entries = kmalloc(DIRECTORY_BYTES_SIZE);
	pread_device((uint8_t*)*dir_entries, DIRECTORY_BYTES_SIZE, BYTES_OFFSET);
	return filter_valid_directory_entries(dir_entries, MAX_ENTRIES);
}

char* fat_file_name_to_str(char* filename_fat_format) {
	const uint32_t FILENAME_LENGTH = 8;
	const uint32_t EXTENSION_LENGTH = 3;

	// Copying:
	char* name = kmalloc(FILENAME_LENGTH + EXTENSION_LENGTH + 2);
	memcpy(name, filename_fat_format, FILENAME_LENGTH);
	if (filename_fat_format[FILENAME_LENGTH] != ' ') {
		name[FILENAME_LENGTH] = '.';
	} else {
		name[FILENAME_LENGTH] = ' ';
	}
	memcpy(name + FILENAME_LENGTH + 1, filename_fat_format + FILENAME_LENGTH, EXTENSION_LENGTH);

	// Removing spaces:
	uint32_t j = 0;
	for (uint32_t i = 0; i < FILENAME_LENGTH + EXTENSION_LENGTH + 1; i++) {
		if (name[i] != ' ') {
			name[j] = tolower(name[i]);
			j++;
		}
	}
	name[j] = '\0';
	return name;
}
