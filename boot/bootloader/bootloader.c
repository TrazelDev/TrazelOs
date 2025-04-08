void bootloader_entry() {
	char* addr = (char*)0xB8000;
	*(addr) = 'A';
	asm volatile("hlt");
}
