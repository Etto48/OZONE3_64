#include "include/syscalls.h"

namespace syscalls
{
    extern "C" void* sys_call(interrupt::context_t* context, uint64_t sys_call_number)
    {
        //rdx = arg0
        multitasking::save_state(context);
        switch (sys_call_number)
        {
        case 0://get_id
            context->rax=multitasking::execution_index;
            break;
        case 1://sleep
            clock::add_timer(context->rdx);
            break;
        case 2://exit
            multitasking::destroy_process(multitasking::execution_index);
            break;
        case 3://create_semaphore
            context->rax = multitasking::create_semaphore(context->rdx);
            break;
        case 4://acquire_semaphore
            multitasking::acquire_semaphore(context->rdx);
            break;
        case 5://release_semaphore
            multitasking::release_semaphore(context->rdx);
            break;
        case 6://fork
            context->rax = multitasking::fork((void(*)())context->rdx);
            break;
        case 7:
            context->rax = (uint64_t)multitasking::process_array[multitasking::execution_index].process_heap.malloc(context->rdx);
            break;
        case 8:
            multitasking::process_array[multitasking::execution_index].process_heap.free((void*)context->rdx);
            break;
        default:
            break;
        }
        return multitasking::load_state();
    }

    extern "C" void* sys_call_system(interrupt::context_t* context, uint64_t sys_call_number)
    {
        //rdx = arg0
        multitasking::save_state(context);
        switch (sys_call_number)
        {
        default:
            break;
        }
        return multitasking::load_state();
    }
};