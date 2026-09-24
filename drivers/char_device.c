#include <include/mem_utils.h>
#include <include/strings.h>
#include <include/types.h>
#include <kernel/include/heap.h>

#include "drivers/char_device.h"

struct multiwrite_dev_private_data {
	size_t devices_num;
	struct char_device** devices;
};

struct duplex_dev_private_data {
	struct char_device* read_dev;
	struct char_device* write_dev;
};

static void setup_device_name(char* device_name, char* name);
static ssize_t multiwrite_dev_write(struct char_device* device, void* buffer, size_t size);
static ssize_t duplex_device_write(struct char_device* device, void* buffer, size_t size);
static ssize_t duplex_device_read(struct char_device* device, void* buffer, size_t size);

struct char_device* char_dev_create_multiwrite_dev(const char* name, struct char_device** devices,
												   size_t num_devices) {
	for (size_t i = 0; i < num_devices; i++) {
		if (!devices[i]->write || !(devices[i]->capabilities & CDC_WRITE_CAPABILITY)) {
			return NULL;
		}
	}

	struct char_device* multiwrite_device = kmalloc(sizeof(struct char_device));
	struct multiwrite_dev_private_data* devices_data =
		kmalloc(sizeof(struct multiwrite_dev_private_data));

	devices_data->devices_num = num_devices;
	devices_data->devices = devices;

	setup_device_name(multiwrite_device->name, (char*)name);
	multiwrite_device->capabilities = CDC_WRITE_CAPABILITY;
	multiwrite_device->private_data = devices_data;
	multiwrite_device->write = multiwrite_dev_write;
	multiwrite_device->read = NULL;
	multiwrite_device->ioctl = NULL;

	return multiwrite_device;
}

struct char_device* char_dev_create_duplex(const char* name, struct char_device* read_dev,
										   struct char_device* write_dev) {
	if (!read_dev || !write_dev) {
		return NULL;
	}
	if (!(read_dev->capabilities & CDC_READ_CAPABILITY) ||
		!(write_dev->capabilities & CDC_WRITE_CAPABILITY)) {
		return NULL;
	}

	struct duplex_dev_private_data* dev_data = kmalloc(sizeof(struct duplex_dev_private_data));
	dev_data->read_dev = read_dev;
	dev_data->write_dev = write_dev;

	struct char_device* duplex_dev = kmalloc(sizeof(struct char_device));
	setup_device_name(duplex_dev->name, (char*)name);
	duplex_dev->capabilities = CDC_WRITE_CAPABILITY | CDC_READ_CAPABILITY;
	duplex_dev->private_data = dev_data;
	duplex_dev->write = duplex_device_write;
	duplex_dev->read = duplex_device_read;
	duplex_dev->ioctl = NULL;

	return duplex_dev;
}

// module private functions:
// -------------------------------------------------------------------------------------------------

static void setup_device_name(char* device_name, char* name) {
	size_t name_len = strlen(name);
	if (name_len >= CHAR_DEVICE_NAME_LEN) {
		name_len = CHAR_DEVICE_NAME_LEN - 1;
	}
	memcpy(device_name, (char*)name, name_len);
	device_name[name_len] = '\0';
}

static ssize_t multiwrite_dev_write(struct char_device* device, void* buffer, size_t size) {
	struct multiwrite_dev_private_data* devices_data =
		((struct multiwrite_dev_private_data*)device->private_data);

	for (size_t i = 0; i < devices_data->devices_num; i++) {
		devices_data->devices[i]->write(devices_data->devices[i], buffer, size);
	}

	return (ssize_t)size;
}

static ssize_t duplex_device_write(struct char_device* device, void* buffer, size_t size) {
	struct duplex_dev_private_data* dev = (struct duplex_dev_private_data*)device->private_data;
	return dev->write_dev->write(dev->write_dev, buffer, size);
}
static ssize_t duplex_device_read(struct char_device* device, void* buffer, size_t size) {
	struct duplex_dev_private_data* dev = (struct duplex_dev_private_data*)device->private_data;
	return dev->read_dev->read(dev->read_dev, buffer, size);
}
