#pragma once
#include <include/vendor/limine.h>

#include "char_device.h"

void init_framebuffer_print(volatile struct limine_framebuffer_response* framebuffer_response);
