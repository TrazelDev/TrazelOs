
void _entry() {
	char* addr = 0xB8000;
	*(addr) = 'A';
	while(1) { int x = 0; } 
}
