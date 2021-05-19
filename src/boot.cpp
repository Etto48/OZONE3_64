#include <stdint.h>
#include "include/multiboot.h"
#include "include/paging.h"
#include "include/printing.h"
#include "include/string_tools.h"

extern "C" void kmain()
{
    paging::init_frame_mapping();
    kprint("Frame mapping initialized\n",0x07);
    char buf[256] = {0};
    string_tools::utoa(paging::kernel_frames,buf);
    kprint("    kernel frames: ",0x07);
    kprint(buf,0x07);
    string_tools::utoa(paging::secondary_frames,buf);
    kprint("\n    secondary frames: ",0x07);
    kprint(buf,0x07);
    kprint("\n    available memory: ",0x07);
    string_tools::utoa(paging::secondary_frames/0x100,buf);
    kprint(buf,0x07);
    kprint("MiB\n",0x07);
}
