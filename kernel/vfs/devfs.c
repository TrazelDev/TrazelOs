#include <drivers/block_device.h>
#include <drivers/char_device.h>
#include <drivers/framebuffer_print.h>
#include <drivers/ps2_keyboard.h>
#include <drivers/serial.h>
#include <drivers/vga_text.h>
#include <include/mem_utils.h>
#include <include/strings.h>
#include <kernel/include/heap.h>
#include <kernel/include/printk.h>
#include <kernel/include/vfs.h>

#include "devfs.h"

#define DEVFS_MAX_DEVICES 256
static struct char_device* g_char_devices[DEVFS_MAX_DEVICES];
static struct block_device* g_block_devices[DEVFS_MAX_DEVICES];
static size_t g_registered_char_devs = 0;

static int64_t char_dev_read(struct vfs_file* vfs_node, uint8_t* buffer, uint64_t size);
static int64_t char_dev_write(struct vfs_file* vfs_node, const uint8_t* buffer, uint64_t size);
static void setup_vfs_node_name(char* vfs_node_name, char* name);
static void devfs_register_char_device(struct char_device* dev);

void init_devfs() {
	for (int i = 0; i < DEVFS_MAX_DEVICES; i++) {
		g_char_devices[i] = NULL;
		g_block_devices[i] = NULL;
	}

	struct char_device* serial_dev = serial_com1_init();
	struct char_device* vga_dev = vga_text_init();
	struct char_device* ps2_dev = ps2_keyboard_init();
	struct char_device* framebuffer_dev = get_framebuffer_print_driver();

	struct char_device** broadcast_dev_list = kmalloc(sizeof(struct char_device*) * 2);
	broadcast_dev_list[0] = serial_dev;
	broadcast_dev_list[1] = framebuffer_dev;
	struct char_device* write_boadcast_dev =
		char_dev_create_multiwrite_dev("write_broadcast", broadcast_dev_list, 2);

	struct char_device* tty_dev =
		char_dev_create_duplex("tty", ps2_keyboard_init(), write_boadcast_dev);

	devfs_register_char_device(serial_dev);
	devfs_register_char_device(vga_dev);
	devfs_register_char_device(ps2_dev);
	devfs_register_char_device(framebuffer_dev);
	devfs_register_char_device(tty_dev);

	printk("Initialized devfs Successfully\n");
}

struct vfs_file* devfs_open(const char* filepath) {
	for (int i = 0; i < g_registered_char_devs; i++) {
		if (g_char_devices[i] != NULL && strcmp(filepath, g_char_devices[i]->name) == 0) {
			struct vfs_file* vfs_node = kmalloc(sizeof(struct vfs_file));
			vfs_node->file_size = -1;
			vfs_node->fs_private_data = g_char_devices[i];
			vfs_node->read = char_dev_read;
			vfs_node->write = char_dev_write;
			vfs_node->close = NULL;
			setup_vfs_node_name(vfs_node->name, g_char_devices[i]->name);
			return vfs_node;
		}
	}

	for (int i = 0; i < DEVFS_MAX_DEVICES; i++) {
		if (g_block_devices[i] != NULL && strcmp(filepath, g_block_devices[i]->name) == 0) {
			// return something
			return NULL;
		}
	}

	return NULL;
}

// module private functions:
// -------------------------------------------------------------------------------------------------

static int64_t char_dev_read(struct vfs_file* vfs_node, uint8_t* buffer, uint64_t size) {
	struct char_device* char_dev = (struct char_device*)vfs_node->fs_private_data;
	if (!char_dev->read) {
		return -1;
	}
	return char_dev->read(char_dev, buffer, size);
}

static int64_t char_dev_write(struct vfs_file* vfs_node, const uint8_t* buffer, uint64_t size) {
	struct char_device* char_dev = (struct char_device*)vfs_node->fs_private_data;
	if (!char_dev->write) {
		return -1;
	}
	return char_dev->write(char_dev, (char*)buffer, size);
}

static void setup_vfs_node_name(char* vfs_node_name, char* name) {
	size_t name_len = strlen(name);
	if (name_len >= VFS_FILE_NAME_LEN) {
		name_len = VFS_FILE_NAME_LEN - 1;
	}
	memcpy(vfs_node_name, (char*)name, name_len);
	vfs_node_name[name_len] = '\0';
}

static void devfs_register_char_device(struct char_device* dev) {
	if (dev && g_registered_char_devs < DEVFS_MAX_DEVICES) {
		g_char_devices[g_registered_char_devs] = dev;
		g_registered_char_devs++;
	}
}
