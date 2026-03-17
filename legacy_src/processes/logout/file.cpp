#define VGA_MEMORY (unsigned char*)0xB8000
int func()
{
	return 0;
}

int main()
{
	*(VGA_MEMORY)	   = 's';
	*(VGA_MEMORY + 2)  = 'u';
	*(VGA_MEMORY + 4)  = 'c';
	*(VGA_MEMORY + 6)  = 'c';
	*(VGA_MEMORY + 8)  = 'e';
	*(VGA_MEMORY + 10) = 's';
	*(VGA_MEMORY + 12) = 's';
	*(VGA_MEMORY + 14) = ' ';
	*(VGA_MEMORY + 16) = 'c';
	*(VGA_MEMORY + 18) = 'o';
	*(VGA_MEMORY + 20) = 't';
	*(VGA_MEMORY + 22) = 'e';
	*(VGA_MEMORY + 24) = 'x';
	*(VGA_MEMORY + 26) = 't';
	*(VGA_MEMORY + 28) = ' ';
	*(VGA_MEMORY + 30) = 's';
	*(VGA_MEMORY + 32) = 'w';
	*(VGA_MEMORY + 34) = 'i';
	*(VGA_MEMORY + 36) = 't';
	*(VGA_MEMORY + 38) = 'c';
	*(VGA_MEMORY + 40) = 'h';
	*(VGA_MEMORY + 42) = '!';


	return 0;
}
