#include "include/syscalls.h"

namespace syscalls
{
    extern "C" void* sys_call(interrupt::context_t* context, uint64_t sys_call_number)
    {
        multitasking::save_state(context);
        switch (sys_call_number)
        {
        case 0://get_id
            context->rax=multitasking::execution_index;
            break;
        case 1://sleep
            multitasking::process_array[multitasking::execution_index].is_ready = false;
            clock::add_timer(multitasking::execution_index,context->rdx);
            break;
        case 2://die
            multitasking::destroy_process(multitasking::execution_index);
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }
        return multitasking::load_state();
    }
};