// This is not the real kernel but one for testing purposes in the refactoring of the bootloader

static void print_kernel_hello(void);

int kmain() {
	print_kernel_hello();
	asm volatile("hlt");
}

void print_kernel_hello(void) {
	volatile unsigned short* vga_buffer = (volatile unsigned short*)0xB8000;
	const char* message = "Hello from kernel!";

	unsigned char color = 0x0F;

	int i = 0;
	while (message[i] != '\0') {
		vga_buffer[i] = (unsigned short)message[i] | (unsigned short)(color << 8);
		i++;
	}
}
