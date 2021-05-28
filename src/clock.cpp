#include "include/clock.h"

namespace clock
{
    constexpr uint16_t COMMAND_PORT = 0x43;
    constexpr uint16_t CLOCK_0 = 0x40;
    constexpr uint16_t CLOCK_1 = 0x41;
    constexpr uint16_t CLOCK_2 = 0x42;

    struct process_timer_t
    {
        uint64_t id;
        uint64_t ticks;

        process_timer_t* next;
    };

    process_timer_t* timer_list = nullptr;

    void set_freq(uint64_t hz)
    {
        int divisor = 1193180 / hz;       /* Calculate our divisor */
        io::outb(COMMAND_PORT, 0x36);             /* Set our command byte 0x36 */
        io::outb(CLOCK_0, divisor & 0xFF);   /* Set low byte of divisor */
        io::outb(CLOCK_0, divisor >> 8);     /* Set high byte of divisor */
    }
    void init()
    {
        set_freq(500);
        interrupt::irq_callbacks[0]=callback;
    }

    void callback(interrupt::context_t* context)
    {
        multitasking::scheduler_timer_ticks++;
        if(timer_list)
        {
            timer_list->ticks--;
            while(timer_list&&timer_list->ticks==0)
            {//run and extract
                multitasking::process_array[timer_list->id].is_ready = true;
                auto to_remove = timer_list;
                timer_list = timer_list->next;
                system_heap.free(to_remove);
            }
        }
    }

    void add_timer(uint64_t id, uint64_t ticks)
    {
        process_timer_t* last = nullptr;
        process_timer_t* p;
        uint64_t time_sum = 0;
        for(
            p = timer_list;
            p && time_sum + p->ticks < ticks;
            time_sum += p->ticks,
            p=p->next)
        {
            last = p;
        }
        process_timer_t* new_process_timer = (process_timer_t*)system_heap.malloc(sizeof(process_timer_t));
        new_process_timer->id = id;
        new_process_timer->next = p;
        new_process_timer->ticks = ticks - time_sum;
        if(p)
        {
            p->ticks-=new_process_timer->ticks;
        }
        if(!last)//head
        {
            timer_list = new_process_timer;
        }
        else//normal
        {
            last->next = new_process_timer;
        }
    }
};