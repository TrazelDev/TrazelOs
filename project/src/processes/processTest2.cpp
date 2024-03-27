#define VGA_MEMORY (unsigned char*)0xB8000

void processTest2()
{
    *(VGA_MEMORY) = 'a';
}