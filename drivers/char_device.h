#pragma once
#include "types.h"

#define CHAR_DEVICE_NAME_LEN 100
struct char_device {
	char name[CHAR_DEVICE_NAME_LEN];
	void* private_data;

	ssize_t (*read)(struct char_device* device, void* buffer, size_t size);
	ssize_t (*write)(struct char_device* device, void* buffer, size_t size);
	ssize_t (*ioctl)(struct char_device* device, uint32_t command, void* arg);
};
