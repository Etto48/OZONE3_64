#include "include/multitasking.h"

namespace multitasking
{
    process_descriptor_t process_array[MAX_PROCESS_NUMBER];
    uint64_t execution_index = 0;

    uint64_t get_available_index()
    {
        for(uint64_t i = 0; i<MAX_PROCESS_NUMBER;i++)
        {
            if(!process_array[i].is_present)
            {
                return i;
            }
        }
        return MAX_PROCESS_NUMBER;
    }

    void init_process_array()
    {
        for(uint64_t i = 0; i<MAX_PROCESS_NUMBER;i++)
        {
            process_array[i].is_present=false;
            process_array[i].id=i;
        }
    }

    void* create_stack(paging::page_table_t* paging_root,interrupt::privilege_level_t privilege_level)
    {
        void* stack_top_address = (void*)0xffffffff00000000;
        uint64_t page_count = 256;
        uint16_t flags = paging::flags::RW;
        if(privilege_level==interrupt::privilege_level_t::user)
            flags|=paging::flags::USER;
        paging::map(stack_top_address,page_count,flags,paging_root,[](void* virtual_address,bool big_pages){return paging::frame_alloc();},false);
        return (void*)((uint64_t)stack_top_address+0x1000*page_count-1);
    }
    void destroy_stack(paging::page_table_t* paging_root)
    {
        void* stack_top_address = (void*)0xffffffff00000000;
        uint64_t page_count = 256;
        paging::unmap(stack_top_address,page_count,paging_root,[](void* phisical_address,bool big_pages){return paging::free_frame(phisical_address);},false);
    }

    uint64_t create_process(void* entrypoint,paging::page_table_t* paging_root,interrupt::privilege_level_t level)
    {
        auto stack_base = create_stack(paging_root,level);
        interrupt::context_t* context = (interrupt::context_t*)paging::virtual_to_phisical((void*)((uint64_t)stack_base-sizeof(interrupt::context_t)),paging_root);
        context->ss = level==interrupt::privilege_level_t::user?interrupt::GDT_USER_DATA_SEGMENT:0;
        context->cs = level==interrupt::privilege_level_t::user?interrupt::GDT_USER_CODE_SEGMENT:interrupt::GDT_SYSTEM_CODE_SEGMENT;
        constexpr uint64_t IF = 0x0200;
        constexpr uint64_t IOPL = 0x3000;
        context->rflags = level==interrupt::privilege_level_t::user?IF:IF|IOPL;
        context->rip = (uint64_t)entrypoint;
        context->rsp = (uint64_t)stack_base;
        context->rbp = (uint64_t)stack_base;
        context->rax = context->rbx = context->rcx = context->rdx = 
            context->rsi = context->rdi =
            context->r8 = context->r9 = context->r10 = context->r11 = context->r12 = context->r13 = context->r14 = context->r15 = context->fs = context->gs = context->int_num = context->int_info = 0;
        
        uint64_t process_id = get_available_index();

        process_array[process_id].id = process_id;
        process_array[process_id].is_present = true;
        process_array[process_id].level = level;
        process_array[process_id].paging_root = paging_root;
        process_array[process_id].context = context;
        process_array[process_id].data_pointer = stack_base;
        return process_id;
    }

    void destroy_process(uint64_t id)
    {
        if(process_array[id].is_present)
        {
            process_array[id].is_present = false;
            destroy_stack(process_array[id].paging_root);
            paging::destroy_paging_trie(process_array[id].paging_root);
        }
    }

    uint64_t scheduler_timer_ticks = 0;
    void scheduler()
    {
        if(scheduler_timer_ticks>=timesharing_interval)
        {
            scheduler_timer_ticks=0;
            while(!process_array[++execution_index].is_present);
        }
    }

    void save_state(interrupt::context_t* context)
    {
        //current process is execution_index if it's valid
        if(execution_index<MAX_PROCESS_NUMBER)
        {//save the current state to the process descriptor
            process_array[execution_index].context = context;
        }
    }
    interrupt::context_t* load_state()
    {
        //we need to call the scheduler to update execution_index
        scheduler();
        //once we know which process to run, update cr3 and stack pointer, the rest of the context
        //will be automatically restored before iret
        paging::set_current_trie(process_array[execution_index].paging_root);//change cr3
        return process_array[execution_index].context;
    }

};