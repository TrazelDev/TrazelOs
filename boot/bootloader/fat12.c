#include <drivers/ata_pio.h>
#include <include/ctype.h>
#include <include/mem_utils.h>
#include <include/types.h>

#include "bootloader_alloc.h"
#include "fat12.h"

static void* (*xmalloc)();
static void (*free)(void*);
static uint64_t fat12_starting_lba_address;

void init_fat12(struct basic_allocator* allocator, uint64_t starting_lba_addressing) {
	xmalloc = allocator->malloc;
	free = allocator->free;
	fat12_starting_lba_address = starting_lba_addressing;
}

void pread_device(uint8_t* buffer, uint64_t read_bytes, int64_t offset) {
	// int deviceFileDescriptor = open(loopDevicePath, O_RDONLY);
	// if (deviceFileDescriptor == -1) {
	// 	perror("Error opening loop device file");
	// 	exit(-1);
	// }

	// ssize_t bytesRead = pread(deviceFileDescriptor, buffer, readBytes, offset);
	// if (bytesRead == -1) {
	// 	perror("File failed to be read");
	// 	close(deviceFileDescriptor);
	// 	exit(-1);
	// }
	// if (bytesRead < readBytes) {
	// 	(void)fprintf(stderr, "pread: file short (%lu out of %lu bytes read)\n", bytesRead,
	// 				  readBytes);
	// 	close(deviceFileDescriptor);
	// 	exit(-1);
	// }

	// close(deviceFileDescriptor);
}

FAT12Header* load_fat12_header(FAT12Header* fat12_header) {
	static char buffer[sizeof(FAT12Header)];

	pread_device((uint8_t*)buffer, sizeof(FAT12Header), 0);
	memcpy(fat12_header, buffer, sizeof(FAT12Header));

	return fat12_header;
}

FAT12Info* load_fat12_info(FAT12Info* fat12_info, FAT12Header* fat12_header) {
	FAT12Info* info = fat12_info;
	uint32_t root_dir_bytes = fat12_header->root_entry_count * sizeof(FAT12DirectoryEntry);

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

uint32_t count_valid_entries(FAT12DirectoryEntry* dir_entries, uint32_t max_entries,
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

uint32_t get_directory_entries(FAT12DirectoryEntry** dirs, FAT12DirectoryEntry* dir_entry,
							   FAT12Info* fat12_info) {
	uint64_t directory_size = get_file_content((uint8_t**)dirs, dir_entry, fat12_info);
	return directory_size;
}

uint32_t get_entries_file_names(char*** file_names, FAT12DirectoryEntry* dir_entries,
								uint32_t dir_entries_count) {
	// Directories names are included in this count:
	uint32_t file_type_entries_count = count_valid_entries(dir_entries, dir_entries_count, false);

	*file_names = (char**)xmalloc(file_type_entries_count * sizeof(char*));
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

uint32_t get_root_file_names(char*** names, FAT12Info* fat12_info) {
	FAT12DirectoryEntry* dir_entries;
	uint32_t entries_count = get_root_directory_entries(&dir_entries, fat12_info);

	uint32_t file_names_count = get_entries_file_names(names, dir_entries, entries_count);
	free(dir_entries);
	return file_names_count;
}

uint32_t filter_valid_directory_entries(FAT12DirectoryEntry** dir_entries, uint32_t entries_count) {
	// Directories names are included in this count:
	uint32_t file_type_entries_count = count_valid_entries(*dir_entries, entries_count, false);

	FAT12DirectoryEntry* filtered_entries =
		xmalloc(file_type_entries_count * sizeof(FAT12DirectoryEntry));
	int valid_index = 0;
	FAT12DirectoryEntry* curr_element;
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

		memcpy(&filtered_entries[valid_index], curr_element, sizeof(FAT12DirectoryEntry));
		valid_index++;
	}
	free(*dir_entries);
	*dir_entries = filtered_entries;
	return file_type_entries_count;
}

uint32_t get_file_content(uint8_t** file_content, FAT12DirectoryEntry* file_directory_entry,
						  FAT12Info* fat12_info) {
	const uint32_t BYTES_PER_CLUSTER =
		fat12_info->bytes_per_sector * fat12_info->sectors_per_cluster;

	uint8_t* fat = get_fat(fat12_info);
	uint32_t file_cluster_count = count_file_clusters(file_directory_entry->first_cluster_id, fat);
	*file_content = xmalloc((uint64_t)file_cluster_count * BYTES_PER_CLUSTER);
	uint8_t* curr_file_content_ptr = *file_content;

	uint32_t curr_cluster_id = file_directory_entry->first_cluster_id;
	char* cluster_data;
	for (uint32_t i = 0; i < file_cluster_count; i++) {
		read_cluster(&cluster_data, curr_cluster_id, fat12_info);
		memcpy(curr_file_content_ptr, cluster_data, BYTES_PER_CLUSTER);
		free(cluster_data);

		curr_file_content_ptr += BYTES_PER_CLUSTER;
		curr_cluster_id = get_next_cluster_id(curr_cluster_id, fat);
	}

	if (is_directory_entry_directory(file_directory_entry)) {
		return BYTES_PER_CLUSTER * file_cluster_count;
	}
	return file_directory_entry->file_size_in_bytes;
}

uint8_t* get_fat(FAT12Info* fat12_info) {
	const uint32_t FAT12_TABLE_SIZE = fat12_info->fat_sector_size * fat12_info->bytes_per_sector;
	const uint32_t FAT_BYTE_OFFSET =
		fat12_info->bytes_per_sector * fat12_info->fat_section_sector_offset;

	uint8_t* fat = xmalloc(FAT12_TABLE_SIZE);
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

uint32_t read_cluster(char** data, uint16_t cluster_id, FAT12Info* fat12_info) {
	uint32_t cluster_num = cluster_id_to_cluster_num(cluster_id);
	uint32_t bytes_per_cluster = fat12_info->sectors_per_cluster * fat12_info->bytes_per_sector;
	uint32_t data_section_sector_offset = cluster_num * fat12_info->sectors_per_cluster;
	uint32_t device_sector_offset =
		fat12_info->data_section_sector_offset + data_section_sector_offset;
	uint32_t device_bytes_offset = device_sector_offset * fat12_info->bytes_per_sector;

	*data = xmalloc(bytes_per_cluster);
	pread_device((uint8_t*)*data, bytes_per_cluster, device_bytes_offset);
	return bytes_per_cluster;
}
uint32_t get_root_directory_entries(FAT12DirectoryEntry** dir_entries, FAT12Info* fat12_info) {
	const uint32_t DIRECTORY_BYTES_SIZE =
		fat12_info->root_dir_sectors_size * fat12_info->bytes_per_sector;
	const uint32_t BYTES_OFFSET = fat12_info->root_dir_sector_offset * fat12_info->bytes_per_sector;
	const uint32_t MAX_ENTRIES = DIRECTORY_BYTES_SIZE / sizeof(FAT12DirectoryEntry);

	*dir_entries = xmalloc(DIRECTORY_BYTES_SIZE);
	pread_device((uint8_t*)*dir_entries, DIRECTORY_BYTES_SIZE, BYTES_OFFSET);
	return filter_valid_directory_entries(dir_entries, MAX_ENTRIES);
}

char* fat_file_name_to_str(char* filename_fat_format) {
	const uint32_t FILENAME_LENGTH = 8;
	const uint32_t EXTENSION_LENGTH = 3;

	// Copying:
	char* name = xmalloc(FILENAME_LENGTH + EXTENSION_LENGTH + 2);
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
