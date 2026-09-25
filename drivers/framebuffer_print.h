#pragma once
#include <include/vendor/limine.h>

#include "char_device.h"

struct char_device* init_framebuffer_print(
	volatile struct limine_framebuffer_response* framebuffer_response);

/** Can only be called after the init_framebuffer_print was called at least once cause it is
 * initlazing the driver with the framebuffer_response */
struct char_device* get_framebuffer_print_driver();
