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
            user::sleep(50);
        }
    }

    /*constexpr uint64_t semaphore_init = 0xFFFFFFFF;
    uint64_t semaphore_id = semaphore_init;
    void test_process5()
    {
        user::sleep(4000);
        test_anim(0x70,75,0);
    }
    void test_process4()
    {
        user::sleep(3000);
        test_anim(0x70,76,0);
    }
    void test_process3()
    {
        user::sleep(2000);
        test_anim(0x70,77,0);
    }
    void test_process2()
    {
        user::sleep(1000);
        test_anim(0x70,78,0);
    }*/
    void test_process()
    {
        //user::sleep(1000);
        //user::fork(test_process2);
        //user::fork(test_process3);
        //user::fork(test_process4);
        //user::fork(test_process5);
        //user::sleep(6000);
        //test_anim(0x70,79,0);
    }
    void nproc()
    {
        while(true)
        {
            printf("Number of processes: %uld    \r",multitasking::process_count);
        }
        printf("\n");
    }
    void init()
    {
        printf("\033c\x0b""Entered multitasking mode\n");
        apic::init();
        printf("\033c\x02""Apic initialized\n");
        clock::init();
        printf("\033c\x02""Clock initialized\n");
        
        //user::fork(nproc);
        user::fork(test_process);

        while(true)
            debug::log(debug::level::inf,"X");
            
    }


};