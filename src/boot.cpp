#include <stdint.h>
#include "./include/multiboot.h"

void write_string( int colour, const char *string )
{
    volatile char *video = (volatile char*)0xB8000;
    while( *string != 0 )
    {
        *video++ = *string++;
        *video++ = colour;
    }
}

extern "C" void lmain(const void* multiboot_struct)
{
    write_string(0x1f,"BEGIN");
    const multiboot_info_t* mb_info = (multiboot_info_t*)multiboot_struct;            /* Make pointer to multiboot_info_t struct */
	multiboot_uint32_t mb_flags = mb_info->flags;                  /* Get flags from mb_info */
 
        void* kentry = nullptr;                                           /* Pointer to the kernel entry point */
 
        if (mb_flags & MULTIBOOT_INFO_MODS) {                          /* Check if modules are available */
                multiboot_uint32_t mods_count = mb_info->mods_count;   /* Get the amount of modules available */
		multiboot_uint32_t mods_addr = mb_info->mods_addr;     /* And the starting address of the modules */
 
                for (uint32_t mod = 0; mod < mods_count; mod++) {
                        multiboot_module_t* module = (multiboot_module_t*)(mods_addr + (mod * sizeof(multiboot_module_t)));     /* Loop through all modules */
                }
        }
    asm volatile("hlt");
}
