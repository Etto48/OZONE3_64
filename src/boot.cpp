#include "include/boot.h"

extern "C" void kmain()
{
    clear(0x07);
    debug::init();
    debug::log(debug::level::inf, "---- OZONE for AMD64 ----");
    printf("\e[47;30mWelcome to OZONE3 for AMD64!\n\e[0m");
    paging::init_frame_mapping(&boot_info::mbi);
    printf("\e[32mFrame mapping initialized\e[0m\n");
    printf("  kernel frames: %uld\n", paging::kernel_frames);
    printf("  secondary frames: %uld\n", paging::secondary_frames);
    printf("  available memory: %uld B\n", paging::free_frames * 0x1000);
    uint64_t ret = (uint64_t)paging::extend_identity_mapping();
    printf("\e[32mIdentity mapping extended\e[0m\n");
    printf("  available memory: %uld B\n", paging::free_frames * 0x1000);
    interrupt::init_interrupts();
    printf("\e[32mInterrupts initialized\e[0m\n");
    multitasking::init_process_array();
    printf("\e[32mProcess array initialized\e[0m\n");

    multitasking::create_process((void *)kernel::init, &paging::identity_l4_table, interrupt::privilege_level_t::system, multitasking::MAX_PROCESS_NUMBER,nullptr);

    if (boot_info::mbi.flags & MULTIBOOT_INFO_MODS && boot_info::mbi.mods_count >= 1)
    {
        printf("%ud module(s) found\n", boot_info::mbi.mods_count);
        for (uint64_t mn = 0; mn < boot_info::mbi.mods_count; mn++)
        {
            auto &modr = ((multiboot_module_t *)(uint64_t)boot_info::mbi.mods_addr)[mn];
            auto ptrie = paging::create_paging_trie();
            auto level = mn == 0 ? interrupt::privilege_level_t::system : interrupt::privilege_level_t::user;
            multitasking::mapping_history_t* mh;
            auto entry = modules::load_module(&modr, ptrie, level, mh);
            if (entry == nullptr)
                printf("  \e[31mModule %uld failed to load\e[0m\n", mn);
            else
            {
                auto pid = multitasking::create_process((void *)entry, ptrie, level, multitasking::MAX_PROCESS_NUMBER, mh);
                printf("  Module %s(%uld) loaded as process %uld\n", modr.cmdline, mn, pid);
            }
        }
    }
    else
    {
        printf("\e[31mcNo modules found\e[0m\n");
    }

    if(boot_info::mbi.flags & MULTIBOOT_INFO_VBE_INFO)
    {
        printf("\e[35mFramebuffer found at 0x%p, %udx%ud colors:%ud\e[0m\n",boot_info::mbi.framebuffer_addr,boot_info::mbi.framebuffer_width,boot_info::mbi.framebuffer_height,boot_info::mbi.framebuffer_palette_num_colors);
    }
    else
    {
        printf("Framebuffer not found\n");
    }

    ozone::user::sys_call_n(0); //create an interrupt to switch to multitasking mode
}
