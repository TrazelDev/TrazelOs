
#define VGA_MEMORY (unsigned char*)0xB8000

int main()
{
    *(VGA_MEMORY) = 'a';
    return 0;
}