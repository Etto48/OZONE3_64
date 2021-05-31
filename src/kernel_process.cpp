#include "include/kernel_process.h"

namespace kernel
{
    void test_anim(uint8_t color,uint8_t x, uint8_t y)
    {
        uint64_t i=0;
        char anim[] = "/-\\|";
        while(true)
        {
            put_char(anim[i],color,x,y);
            i++;
            i%=4;
            sys::sleep(50);
        }
    }

    constexpr uint64_t semaphore_init = 0xFFFFFFFF;
    uint64_t semaphore_id = semaphore_init;
    void test_process5()
    {
        sys::sleep(4000);
        test_anim(0x70,75,0);
    }
    void test_process4()
    {
        sys::sleep(3000);
        test_anim(0x70,76,0);
    }
    void test_process3()
    {
        sys::sleep(2000);
        test_anim(0x70,77,0);
    }
    void test_process2()
    {
        sys::sleep(1000);
        test_anim(0x70,78,0);
    }
    void test_process()
    {
        test_anim(0x70,79,0);
    }
    void init()
    {
        printf("\n\033c\x0b""Entered multitasking mode");
        apic::init();
        printf("\n\033c\x02""Apic initialized");
        clock::init();
        printf("\n\033c\x02""Clock initialized");
        
        while(true);//idle in case of need;
    }


};