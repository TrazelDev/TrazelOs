#pragma once
#include <include/types.h>

#include "bootloader_alloc.h"

void init_fat12(struct basic_allocator* allocator, uint64_t starting_lba_addressing);

typedef struct FAT12Header {
	// BPB:
	uint8_t bootjmp[3];
	uint8_t oem_name[8];
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sector_count;	 // The number of reserved sectors
	uint8_t table_count;
	uint16_t root_entry_count;
	uint16_t total_sectors16;
	uint8_t media_type;
	uint16_t table_size16;	// The number of sectors per FAT
	uint16_t sectors_per_track;
	uint16_t head_side_count;
	uint32_t hidden_sector_count;
	uint32_t total_sectors32;

	// fat12/fat16 specifics:
	uint8_t bios_drive_num;
	uint8_t reserved1;
	uint8_t boot_signature;
	uint32_t volume_id;
	uint8_t volume_label[11];
	uint8_t fat_type_label[8];
} __attribute__((packed)) FAT12Header;

typedef struct FAT12DirectoryEntry {
	char file_name[11];
	uint8_t attributes;
	uint8_t reserved;
	uint8_t creation_time_centiseconds;
	uint16_t creation_time;
	uint16_t creation_date;
	uint16_t last_access_date;
	uint16_t zero_value;  // In fat12 and fat16 this is always zero
	uint16_t last_modify_time;
	uint16_t last_modify_date;
	uint16_t first_cluster_id;
	uint32_t file_size_in_bytes;
} __attribute__((packed)) FAT12DirectoryEntry;
#define FINAL_ENTRY 0x00
#define DELETED_ENTRY 0xE5
#define VOLUME_LABEL_ATTRIBUTE 0x08
#define FAT12_ATTR_DIRECTORY 0x10

static inline bool is_final_directory_entry(FAT12DirectoryEntry* entry) {
	return entry->file_name[0] == FINAL_ENTRY;
}
static inline bool is_volume_label_entry(FAT12DirectoryEntry* entry) {
	return entry->attributes & VOLUME_LABEL_ATTRIBUTE;
}
static inline bool is_deleted_entry(FAT12DirectoryEntry* entry) {
	return ((uint8_t)entry->file_name[0] == DELETED_ENTRY);
}
static inline bool is_directory_entry_directory(const FAT12DirectoryEntry* entry) {
	return (entry->attributes & FAT12_ATTR_DIRECTORY) != 0;
}

#define FAT_LAST_CLUSTER_NUM 0xFFF
typedef struct FAT12Info {
	uint32_t bytes_per_sector;
	uint32_t sectors_per_cluster;
	uint32_t total_sectors;
	uint32_t fat_sector_size;
	uint32_t fat_section_sector_size;
	uint32_t root_dir_sectors_size;
	uint32_t data_sectors_size;
	uint32_t cluster_count;

	// the offset of sections:
	uint32_t data_section_sector_offset;
	uint32_t fat_section_sector_offset;
	uint32_t root_dir_sector_offset;
} FAT12Info;

/** Reads bytes from a loopDevice from an offset and loads into a preallocated buffer.
 * In case the function fails to read it exists out of the program.
 * @param[in] buffer preallocated buffer the caller provides.
 * @param[in] readBytes Number of bytes to read from loop device.
 * @param[in] offset the offset to start reading from the device.
 * @param[in] loopDevicePath Path to loop device that will be read
 */
void pread_device(uint8_t* buffer, uint64_t read_bytes, int64_t offset);

/** Loads FAT12Header with information from a loop device.
 * @param[out] fat12Header Pointer to the allocated structure to load information to.
 * @param[in] loopDevicePath
 * @return A pointer to fat12Header.
 */
FAT12Header* load_fat12_header(FAT12Header* fat12_header);

/** Loads FAT12Info from FAT12Header.
 * @param[out] fat12Info Pointer to the allocated structure to load information to.
 * @param[in] fat12Header A FAT12Header structure with correct information of some loop device.
 * @return A pointer fat12Info.
 */
FAT12Info* load_fat12_info(FAT12Info* fat12_info, FAT12Header* fat12_header);

/** Gets the root folder file names in an array from the loopDevice using FAT12Info
 *
 * @param[out] names Address of char** variable. The function will allocate it.
 * @note Caller will free each string in the array then the array itself.
 * @param[in] fat12Info A FAT12Info that was created from the loopDevice.
 * @param[in] loopDevicePath.
 *
 * @return The count of file names in names variable.
 */
uint32_t get_root_file_names(char*** names, FAT12Info* fat12_info);

/**
 * @brief Gets file names from an array of FAT12 directory entries.
 * This function extracts file names from the provided directory entries and stores them in a
 * dynamically allocated array of strings.
 *
 * @param[out] fileNames Address of a char** variable. The function will allocate memory for the
 * array and for each file name string. The caller is responsible for freeing each string in the
 * array and then the array itself.
 * @param[in] dirEntries Pointer to an array of FAT12DirectoryEntry structures.
 * @param[in] dirEntriesCount Number of directory entries in the dirEntries array.
 *
 * @return The count of file names stored in the fileNames variable.
 */
uint32_t get_entries_file_names(char*** file_names, FAT12DirectoryEntry* dir_entries,
								uint32_t dir_entries_count);

/** Extracts fat12 directory entries of specific directory from the loopDevice provided.
 * This entries include:
 * directories, files, deleted entries, long file name entries and volume label entries.
 *
 * @param[out] dirs Pointer to an array of FAT12DirectoryEntry that will be allocated internally.
 * @note Caller will free the array.
 * @param[in] dirEntry The directory entry of that directory
 * @param[in] loopDevicePath
 *
 * @return Amount of directory entries in variable dirs.
 */
uint32_t get_directory_entries(FAT12DirectoryEntry** dirs, FAT12DirectoryEntry* dir_entry,
							   FAT12Info* fat12_info);

/** @brief Filters a FAT12 directory array and keeps only file and directory entries.
 *
 * Function reads entries from *dirEntries and allocates a new array into *dirEntries after a
 * filter.
 *
 * @param[in,out] dirEntries Pointer to an array of FAT12DirectoryEntry that already contains
 * entries, on input holds pointer to an unfiltered array of FAT12DirectoryEntries, with
 * entriesCount elements, while on output holds pointer to a filtered array of
 * FAT12DirectoryEntries.
 * @note Input array freed by function and output array is then freed by caller.
 * @param[in] entriesCount Amuont of entries in *dirEntries.
 *
 * @return Number of entries in the new *dirEntries
 */
uint32_t filter_valid_directory_entries(FAT12DirectoryEntry** dir_entries, uint32_t entries_count);

/**
 * @brief Reads the contents of a FAT12 file into a newly allocated buffer.
 *
 * This function reads the file described by fileDirectoryEntry from the FAT12 filesystem
 * described by fat12Info located at loopDevicePath. The file data is returned via fileContent
 * and the size of it as return value. The caller owns the returned buffer.
 * @note Caller is responsible to verify file is not empty
 *
 * @param[out] fileContent On success, set to a newly allocated buffer containing the file's bytes.
 * @note The caller must free the buffer.
 * @param[in] fileDirectoryEntry Directory entry describing the file to read.
 * @param[in] fat12Info
 * @param[in] loopDevicePath Path to loop device.
 *
 * @return Number of bytes written to fileContent on success. Returns 0 on failure.
 */
uint32_t get_file_content(uint8_t** file_content, FAT12DirectoryEntry* file_directory_entry,
						  FAT12Info* fat12_info);

/** Loads the FAT (File Allocation Table) from a FAT12 loopDevice.
 * The function reads the first FAT in the FAT section of the filesystem located in loopDevice.
 * @param[in] fat12Info
 * @param[in] loopDevicePath Path to filesystem loop device.
 * @return Fat loaded into memory.
 * @note Caller will free the returned fat.
 */
uint8_t* get_fat(FAT12Info* fat12_info);

/**
 * @brief Reads a single FAT12 cluster into a newly allocated buffer.
 * This function read the cluster identified by clusterId from the fat12 filesystem located on the
 * loop device loopDevicePath points to.
 *
 * @param[out] data Set to a newly allocated buffer containing the clusters bytes.
 * @note Caller will free the memory
 * @param[in] clusterId Cluster id to read (clusterId=2 means the first cluster which internally
 * converted to cluster number).
 * @param[in] fat12Info
 * @param[in] loopDevicePath
 *
 * @return Number of bytes written to data (probably cluster size).
 */
uint32_t read_cluster(char** data, uint16_t cluster_id, FAT12Info* fat12_info);

/** Extracts FAT12 root directory entries from the loopDevice provided.
 * This directory entries only include: directories, files
 *
 * @param[out] dirs Pointer to an array of FAT12DirectoryEntry that will be allocated internally.
 * @note Caller will free the array.
 * @param[in] fat12Info A FAT12Info that was created from the loopDevice.
 * @param[in] loopDevicePath Path to filesystem loop device.
 *
 * @return Amount of root directory entries in variable dirs.
 */
uint32_t get_root_directory_entries(FAT12DirectoryEntry** dir_entries, FAT12Info* fat12_info);

/** Counts clusters for a specific cluster chain from the fat 12 table*/
uint32_t count_file_clusters(uint16_t initial_cluster_id, const uint8_t* fat);
/** Gets a cluster id and fat 12 and returns next cluster in the chain.
 @note No error handling assumes values are correct.
 */
uint16_t get_next_cluster_id(uint16_t cluster_id, const uint8_t* fat);
/** Converts clusterId to cluster number since the first id is 2 which points to cluster 0 */
static inline uint32_t cluster_id_to_cluster_num(uint16_t cluster_id) { return cluster_id - 2; }
static inline uint32_t bytes_to_sectors_round_up(uint32_t bytes, uint16_t bytes_per_sector) {
	return (bytes + bytes_per_sector - 1) / bytes_per_sector;
}

/** converts filename in the format of fat12 to a regular file name, handles errors internally and
 * terminates the program on fail
 * @param a file name in the format of fat12(11 chars long [8 for name][3 for extension])
 * @return an allocated string containing the name
 */
char* fat_file_name_to_str(char* filename_fat_format);
