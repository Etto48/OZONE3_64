#pragma once
#include <stdint.h>
#include <stddef.h>

namespace user
{
    uint64_t sys_call_n(uint64_t sys_call_number,uint64_t arg0 = 0);

    //returns the id of the current process
    uint64_t get_id();
    //stops the process for <ticks> ticks of clock
    void sleep(uint64_t ticks);
    //kills the current process
    void exit();
    //allocates a semaphore to manage access to shared resources
    uint64_t create_semaphore(uint64_t start_count);
    //request access to a shared resource
    void acquire_semaphore(uint64_t semaphore_id);
    //releases the access to a shared resource 
    void release_semaphore(uint64_t semaphore_id);
    //creates a child process which inherits the pagigng trie (stack excluded) and has entrypoint main
    //returns the process_id of the new process, user::exit() will be automatically called if the functions returns
    uint64_t fork(void (*main)());
    //prints a line
    void println(const char* str);
};
void* operator new(size_t size);
//void* operator new(size_t size, size_t align);
void operator delete(void* address);

namespace system
{
    uint64_t sys_call_n(uint64_t sys_call_number,uint64_t arg0 = 0,uint64_t arg1 = 0,uint64_t arg2 = 0,uint64_t arg3 = 0);
};