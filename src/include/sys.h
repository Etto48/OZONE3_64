#pragma once
#include <stdint.h>

namespace sys
{
    void sys_call_n(uint64_t sys_call_number,uint64_t arg0 = 0);

    //returns the id of the current process
    uint64_t get_id();
    //stops the process for <ticks> ticks of clock
    void sleep(uint64_t ticks);
    //kills the current process
    void die();
};