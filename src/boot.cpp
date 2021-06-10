#include "include/boot.h"

extern "C" void kmain()
{
    clear(0x07);
    debug::init();
    debug::log(debug::level::inf, "---- OZONE for AMD64 ----");
    printf("\033c\x70Welcome to OZONE3 for AMD64!\n");
    paging::init_frame_mapping(&boot_info::mbi);
    printf("\033c\x02"
           "Frame mapping initialized\n");
    printf("  kernel frames: %uld\n", paging::kernel_frames);
    printf("  secondary frames: %uld\n", paging::secondary_frames);
    printf("  available memory: %uld B\n", paging::free_frames * 0x1000);
    uint64_t ret = (uint64_t)paging::extend_identity_mapping();
    printf("\033c\x02Identity mapping extended\n");
    printf("  available memory: %uld B\n", paging::free_frames * 0x1000);
    interrupt::init_interrupts();
    printf("\033c\x02Interrupts initialized\n");
    multitasking::init_process_array();
    printf("\033c\x02Process array initialized\n");

    multitasking::create_process((void *)kernel::init, &paging::identity_l4_table, interrupt::privilege_level_t::system, multitasking::MAX_PROCESS_NUMBER);

    if (boot_info::mbi.flags & MULTIBOOT_INFO_MODS && boot_info::mbi.mods_count >= 1)
    {
        printf("%ud module(s) found\n", boot_info::mbi.mods_count);
        for (uint64_t mn = 0; mn < boot_info::mbi.mods_count; mn++)
        {
            auto &modr = ((multiboot_module_t *)(uint64_t)boot_info::mbi.mods_addr)[mn];
            auto ptrie = paging::create_paging_trie();
            auto level = mn == 0 ? interrupt::privilege_level_t::system : interrupt::privilege_level_t::user;
            auto entry = modules::load_module(&modr, ptrie, level);
            if (entry == nullptr)
                printf("  \033c\x0cModule %uld failed to load\n", mn);
            else
            {
                auto pid = multitasking::create_process((void *)entry, ptrie, level, multitasking::MAX_PROCESS_NUMBER);
                printf("  Module %s(%uld) loaded as process %uld\n", modr.cmdline, mn, pid);
            }
        }
    }
    else
    {
        printf("\033c\x0cNo modules found\0330\n");
    }

    user::sys_call_n(0); //create an interrupt to switch to multitasking mode
}
