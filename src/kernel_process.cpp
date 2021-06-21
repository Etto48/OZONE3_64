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
        printf("\e[0;1H");
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
    void startup()
    {
        video::draw_image(ozone_logo,{256,256},video::get_screen_size()/2-video::v2i{256,256}/2);
        printf("\e[%uld;%uldH[",(video::get_screen_size()/8/2).x-11,(video::get_screen_size()/8/2).y+8);
        printf("\e[%uld;%uldH]",(video::get_screen_size()/8/2).x+10,(video::get_screen_size()/8/2).y+8);
        for(uint64_t i = 1; i < 21;i++)
        {
            printf("\e[%uld;%uldH=",(video::get_screen_size()/8/2).x-11+i,(video::get_screen_size()/8/2).y+8);
            ozone::user::sleep(200);
        }
        printf("\e[%uld;%uldHWelcome to OZONE3 - 64",(video::get_screen_size()/8/2).x-11,(video::get_screen_size()/8/2).y+8);
        ozone::user::sleep(1000);
        printf("\e[2J\e[H");
        ozone::user::fork(status);
        ozone::user::fork(test_ret);
        while(true);
    }
    void init()
    {
        //printf("\e[36mEntered multitasking mode\e[0m\n");
        apic::init();
        //printf("\e[32mApic initialized\e[0m\n");
        clock::init();
        //printf("\e[32mClock initialized\e[0m\n");
        //printf("\e[2J");
        ozone::user::fork(startup);

        while (true)
            asm volatile("hlt");
    }

};