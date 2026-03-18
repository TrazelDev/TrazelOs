#include <drivers/vga_text.h>
#include <include/integer_utils.h>
#include <include/strings.h>

#include "printk.h"

#define SPECIAL_SYMBOL_PREFIX '%'
enum special_symbols {
	SYMBOL_CHAR = 'c',
	SYMBOL_INTEGER = 'd',
	SYMBOL_STRING = 's',
	SYMBOL_UNSIGNED_INTEGER = 'u',
	SYMBOL_HEX = 'x',
};

static struct char_device* g_ch_device;
void init_printk() { g_ch_device = vga_text_init(); }

static bool is_special_symbol(char chr);
static void print_string(char* val);
static void print_char(char val);
static void print_int(int64_t val);
static void print_unsignedint(uint64_t val);
static void print_hex(uint64_t val);

// Since infinite params are creating a weird stack frame then we need to add the following when we
// want to get the second param from the first on. Also there can only be max of around 20 params
// and the function is compiled with optimization level of 0 to make the stack frame constant in all
// configurations
#define AMOUNT_TO_GET_FIRST_PARAM 4

uint32_t __attribute__((optimize("-O0"))) printk(const char* format, ...) {
	void* param = ((&format) + AMOUNT_TO_GET_FIRST_PARAM);

	while (*format != NULL) {
		if (*format == SPECIAL_SYMBOL_PREFIX && is_special_symbol(*(format + 1))) {
			switch (*(format + 1)) {
				case SYMBOL_CHAR:
					print_char(*((char*)param));
					break;
				case SYMBOL_INTEGER:
					print_int(*((int32_t*)param));
					break;
				case SYMBOL_STRING:
					print_string(*((char**)param));
					break;
				case SYMBOL_UNSIGNED_INTEGER:
					print_unsignedint(*((uint64_t*)param));
					break;
				case SYMBOL_HEX:
					print_hex(*((uint64_t*)param));
					break;
				default:
					break;
			}

			// adding the size of address to go to the next address
			param = ((uint8_t*)param) + sizeof(uint64_t);

			// since this special chars are 2 chars % and something else
			format += 2;
			continue;
		}

		print_char(*format);

		// going to the next char:
		format++;
	}

	return 0;
}

static bool is_special_symbol(char chr) {
	switch (chr) {
		case SYMBOL_CHAR:
		case SYMBOL_INTEGER:
		case SYMBOL_STRING:
		case SYMBOL_UNSIGNED_INTEGER:
		case SYMBOL_HEX:
			return true;
		default:
			return false;
	}
}

static void print_string(char* val) { g_ch_device->write(g_ch_device, val, strlen(val)); }
static void print_char(char val) { g_ch_device->write(g_ch_device, &val, 1); }

#define MAX_BUFFER_SIZE 30
static void print_int(int64_t val) {
	char buffer[MAX_BUFFER_SIZE];
	itoa_signed(val, buffer, INTEGER_BASE_DECIMAL);

	g_ch_device->write(g_ch_device, buffer, strlen(buffer));
}
static void print_unsignedint(uint64_t val) {
	char buffer[MAX_BUFFER_SIZE];
	itoa_unsigned(val, buffer, INTEGER_BASE_DECIMAL);

	g_ch_device->write(g_ch_device, buffer, strlen(buffer));
}
static void print_hex(uint64_t val) {
	char buffer[MAX_BUFFER_SIZE];
	itoa_unsigned(val, buffer, INTEGER_BASE_HEX);

	g_ch_device->write(g_ch_device, buffer, strlen(buffer));
}
#undef MAX_BUFFER_SIZE
