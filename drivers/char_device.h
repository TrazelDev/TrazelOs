#pragma once
#include "types.h"

enum char_driver_capabilities {
	CDC_NO_CAPABILITIES = 0,
	CDC_READ_CAPABILITY = (1 << 0),
	CDC_WRITE_CAPABILITY = (1 << 1),
};

#define CHAR_DEVICE_NAME_LEN 100
struct char_device {
	char name[CHAR_DEVICE_NAME_LEN];
	enum char_driver_capabilities capabilities;
	void* private_data;

	ssize_t (*read)(struct char_device* device, void* buffer, size_t size);
	ssize_t (*write)(struct char_device* device, void* buffer, size_t size);
	ssize_t (*ioctl)(struct char_device* device, uint32_t command, void* arg);
};

/** Takes a name, pointer to an array of pointers to char devices and the number of devices.
 * @param name the name for the device
 * @param devices pointer to an array of pointers to char devices
 * @param num_devices the number of devices in the array
 * @return a new char device that can write to multiple write char devices at once.
 */
struct char_device* char_dev_create_multiwrite_dev(const char* name, struct char_device** devices,
												   size_t num_devices);
