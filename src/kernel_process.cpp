#include "include/kernel_process.h"

namespace kernel
{
    void test_anim(uint8_t color, uint8_t x, uint8_t y)
    {
        uint64_t i = 0;
        char anim[] = "/-\\|";
        while (true)
        {
            put_char(anim[i], color, x, y);
            i++;
            i %= 4;
            ozone::user::sleep(100);
        }
    }

    void test_ret()
    {
        while (true)
        {
            printf("%c", keyboard::getc());
        }
    }
    void status()
    {
        char anim[] = "|/-\\";
        uint8_t i = 0;
        while (true)
        {
            printf("\e[s\e[H\e[30;47m\e[2KOZONE\e[40;0HProc:%uld FreeMem:%uld/%uldKiB\e[10000C%c\e[u\e[0m", multitasking::process_count, (paging::free_frames * 0x1000) / (1024),paging::system_memory/(1024), anim[i]);
            ozone::user::sleep(500);
            i++;
            i %= 4;
        }
    }
    void init()
    {
        printf("\e[36mEntered multitasking mode\e[0m\n");
        apic::init();
        printf("\e[32mApic initialized\e[0m\n");
        clock::init();
        printf("\e[32mClock initialized\e[0m\n");

        ozone::user::fork(status);
        ozone::user::fork(test_ret);

        while (true)
            asm volatile("hlt");
    }

};