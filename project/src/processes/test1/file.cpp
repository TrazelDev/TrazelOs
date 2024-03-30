#define VGA_MEMORY (unsigned char*)0xB8000

int func()
{
    return 0;
}
int main()
{
    *(VGA_MEMORY) = 'n';
    // *(VGA_MEMORY) = 'b';
    // *(VGA_MEMORY) = 'c';

    //asm volatile("hlt");      
    return 0;
}