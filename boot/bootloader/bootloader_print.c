#include <drivers/vga_text.h>
#include <include/strings.h>

#include "bootloader_print.h"

void (*print_string)(char* str);
static struct char_device* ch_device;

static void print(char* str) { ch_device->write(ch_device, str, strlen(str)); }

void init_print() {
	print_string = print;
	ch_device = vga_text_init();
}
