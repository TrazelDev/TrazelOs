#define VGA_MEMORY (unsigned char*)0xB8000
int func()
{
    return 0;
}

int main()
{
    *(VGA_MEMORY)     = 'a';
    asm volatile("hlt");
    //*(VGA_MEMORY + 2) = 'b';
    //*(VGA_MEMORY + 4) = ' ';
    //*(VGA_MEMORY + 6) = ' ';
    //*(VGA_MEMORY + 8) = ' ';
    return 0;
}