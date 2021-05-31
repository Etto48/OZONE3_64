#include "include/ozone/sys.h"

namespace sys
{
    uint64_t sys_call_n(uint64_t sys_call_number,uint64_t arg0)
    {
        uint64_t ret;
        asm volatile("mov %0, %%rdx"::"r"(arg0));
        asm volatile("mov %0, %%rsi"::"r"(sys_call_number));
        asm volatile("int $0x80");
        asm volatile("mov %%rax, %0" : "=r"(ret));
        return ret;
    }
    uint64_t get_id()
    {
        return sys_call_n(0);
    }
    void sleep(uint64_t ticks)
    {
        sys_call_n(1,ticks);
    }
    void die()
    {
        sys_call_n(2);
    }
    uint64_t create_semaphore(uint64_t start_count)
    {
        return sys_call_n(3,start_count);
    }
    void acquire_semaphore(uint64_t semaphore_id)
    {
        sys_call_n(4,semaphore_id);
    }
    void release_semaphore(uint64_t semaphore_id)
    {
        sys_call_n(5,semaphore_id);
    }
};