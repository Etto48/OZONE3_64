#pragma once
#include "interrupt.h"
#include "multitasking.h"

namespace syscalls
{
    extern "C" void* sys_call(interrupt::context_t* context, uint64_t sys_call_number);
};