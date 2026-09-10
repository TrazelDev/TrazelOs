#pragma once
#include <include/types.h>
#include <include/vendor/limine.h>

void init_printk(volatile struct limine_framebuffer_response* framebuffer_response);
uint32_t printk(const char* format, ...);
