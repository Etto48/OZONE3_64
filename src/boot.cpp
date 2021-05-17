#include <stdint.h>

extern "C" void kernel_main()
{
    char* vga_text = (char*)0xb8000;
    for(uint32_t i=0;i<80*25*2;i++)
    {
        vga_text[i]=0;
    }
    asm volatile("hlt");
}