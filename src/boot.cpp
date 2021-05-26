#include <stdint.h>
#include "include/multiboot.h"
#include "include/paging.h"
#include "include/printing.h"
#include "include/string_tools.h"
#include "include/interrupt.h"
#include "include/multitasking.h"

extern "C" multiboot_info_t mbi;

extern "C" void kmain()
{
    clear(0x07);
    printf("\033c\x70Welcome to OZONE3 for AMD64!\n");
    if(mbi.flags & MULTIBOOT_INFO_MODS && mbi.mods_count >= 1)
    {
        printf("%ud module(s) found",mbi.mods_count);
    }
    else
    {
        printf("\033c\x0cNo modules found\0330");
    }
    paging::init_frame_mapping(&mbi);
    printf("\n\033c\x02""Frame mapping initialized");
    printf("\n  kernel frames: %uld",paging::kernel_frames);
    printf("\n  secondary frames: %uld",paging::secondary_frames);
    printf("\n  available memory: %uld B",paging::free_frames*0x1000);
    uint64_t ret = (uint64_t)paging::extend_identity_mapping();
    printf("\n\033c\x02Identity mapping extended");
    printf("\n  available memory: %uld B",paging::free_frames*0x1000);
    interrupt::init_interrupts();
    printf("\n\033c\x02Interrupts initialized");
    multitasking::init_process_array();
    printf("\n\033c\x02Process array initialized");
    
}
