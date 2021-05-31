#pragma once
#include <stdint.h>

namespace sys
{
    uint64_t sys_call_n(uint64_t sys_call_number,uint64_t arg0 = 0);

    //returns the id of the current process
    uint64_t get_id();
    //stops the process for <ticks> ticks of clock
    void sleep(uint64_t ticks);
    //kills the current process
    void die();
    //allocates a semaphore to manage access to shared resources
    uint64_t create_semaphore(uint64_t start_count);
    //request access to a shared resource
    void acquire_semaphore(uint64_t semaphore_id);
    //releases the access to a shared resource 
    void release_semaphore(uint64_t semaphore_id);
};