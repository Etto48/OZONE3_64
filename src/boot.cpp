#include <stdint.h>
#include "include/multiboot.h"
#include "include/paging.h"
#include "include/printing.h"
#include "include/string_tools.h"

extern "C" void kmain(multiboot_info_t* mbi)
{
    clear(0x07);
    kprint("Welcome to OZONE3 for AMD64!\n",0x2f);
    paging::init_frame_mapping();
    kprint("Frame mapping initialized\n",0x07);
    char buf[256] = {0};
    string_tools::utoa(paging::kernel_frames,buf);
    kprint(">kernel frames: ",0x07);
    kprint(buf,0x07);
    string_tools::utoa(paging::secondary_frames,buf);
    kprint("\n>secondary frames: ",0x07);
    kprint(buf,0x07);
    kprint("\n>available memory: ",0x07);
    string_tools::utoa(paging::free_frames*0x1000,buf);
    kprint(buf,0x07);
    kprint("B\n",0x07);

    uint64_t ret = (uint64_t)paging::extend_identity_mapping();
    kprint("Identity mapping extended to cover entire available memory",0x07);
    kprint("\n>available memory: ",0x07);
    string_tools::utoa(paging::free_frames*0x1000,buf);
    kprint(buf,0x07);
    kprint("B\n",0x07);

}
