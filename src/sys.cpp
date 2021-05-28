#include "include/sys.h"

namespace sys
{
    void sys_call_n(uint64_t sys_call_number,uint64_t arg0)
    {
        asm volatile("mov %0, %%rdx"::"r"(arg0));
        asm volatile("mov %rdi, %rsi");
        asm volatile("int $0x80");
    }
    uint64_t get_id()
    {
        sys_call_n(0);
        uint64_t ret;
        asm volatile("mov %%rax, %0" : "=r"(ret));
        return ret;
    }
    void sleep(uint64_t ticks)
    {
        sys_call_n(1,ticks);
    }
    void die()
    {
        sys_call_n(2);
    }
};