#include "ozone.h"

namespace user
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
    void exit()
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
    uint64_t fork(void (*main)())
    {
        return sys_call_n(6,(uint64_t)main);
    }
    /*void println(const char* str)
    {
        sys_call_n(9,(uint64_t)str);
    }*/
};

void* operator new(size_t size)
{
    return (void*)user::sys_call_n(7,(uint64_t)size);
}
//void* operator new(size_t size, size_t align)
//{
//}
void operator delete(void* address)
{
    user::sys_call_n(8,(uint64_t)address);
}

namespace system
{
    uint64_t sys_call_n(uint64_t sys_call_number,uint64_t arg0,uint64_t arg1,uint64_t arg2,uint64_t arg3)
    {
        uint64_t ret;
        asm volatile("mov %0, %%rdx"::"r"(arg0));
        asm volatile("mov %0, %%rcx"::"r"(arg1));
        asm volatile("mov %0, %%r8"::"r"(arg2));
        asm volatile("mov %0, %%r9"::"r"(arg3));
        asm volatile("mov %0, %%rsi"::"r"(sys_call_number));
        asm volatile("int $0x81");
        asm volatile("mov %%rax, %0" : "=r"(ret));
        return ret;
    }
};