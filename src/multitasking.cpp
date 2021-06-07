#include "include/multitasking.h"

namespace multitasking
{
    process_descriptor_t process_array[MAX_PROCESS_NUMBER];
    semaphore_descriptor_t semaphore_array[MAX_SEMAPHORE_NUMBER];
    volatile uint64_t execution_index = MAX_PROCESS_NUMBER;//for forcing the scheduler to initialize
    volatile uint64_t process_count = 0;

    process_descriptor_t* ready_queue = nullptr;
    process_descriptor_t* last_ready = nullptr;

    extern "C"
    { 
        uint64_t sys_stack_base = system_stack_bottom_address;
    }

    void print_queue()
    {
        for(auto p = ready_queue; p; p=p->next)
        {
            printf("%ld ",p->id);
        }
        printf("\n");
    }

    void add_to_list(uint64_t process_id,process_descriptor_t*& head, process_descriptor_t*& tail)
    {
        if(process_array[process_id].is_present)
        {
            auto p = &process_array[process_id];
            if(!tail)
            {
                head = p;
            }
            else
            {
                tail->next = p;
            }
            tail = p;
            p->next = nullptr;
        }
    }

    uint64_t pop_from_list(process_descriptor_t*& head,process_descriptor_t*& tail)
    {
        if(!head)
            abort("Process requested but queue is empty");
        auto ret = head->id;
        if(head==tail)
            tail = nullptr;
        head = head->next;
        return ret;
    }

    uint64_t pop_ready()//pop from top
    {
        //printf("POP    ");
        //print_queue();
        return pop_from_list(ready_queue,last_ready);
    }
    void add_ready(uint64_t process_id)//add to tail
    {
        //printf("ADD %ld  ",process_id);
        //print_queue();
        add_to_list(process_id,ready_queue,last_ready);
    }

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

    uint64_t get_available_semaphore()
    {
        for(uint64_t i = 0; i<MAX_SEMAPHORE_NUMBER;i++)
        {
            if(!semaphore_array[i].is_present)
            {
                return i;
            }
        }
        return MAX_SEMAPHORE_NUMBER;
    }

    uint64_t create_semaphore(int64_t starting_count)
    {
        auto id = get_available_semaphore();
        semaphore_array[id].is_present = true;
        semaphore_array[id].count = starting_count;
        semaphore_array[id].waiting_list_head = nullptr;
        semaphore_array[id].waiting_list_tail = nullptr;
        semaphore_array[id].creator_id = execution_index;
        return id;
    }

    void acquire_semaphore(uint64_t semaphore_id)
    {
        if(semaphore_array[semaphore_id].is_present)
        {
            semaphore_array[semaphore_id].count--;
            if(semaphore_array[semaphore_id].count<0)//move the current process into the waiting list
            {
                auto to_add = execution_index;
                drop();
                add_to_list(to_add,semaphore_array[semaphore_id].waiting_list_head,semaphore_array[semaphore_id].waiting_list_tail);
            }
        }
        else
        {
            abort("Semaphore no more present, maybe the creator process was closed?");
        }
    }
    void release_semaphore(uint64_t semaphore_id)
    {
        if(semaphore_array[semaphore_id].is_present)
        {
            semaphore_array[semaphore_id].count++;
            if(semaphore_array[semaphore_id].count<=0)
            {
                add_ready(pop_from_list(semaphore_array[semaphore_id].waiting_list_head,semaphore_array[semaphore_id].waiting_list_tail));
            }
        }
    }

    uint64_t fork(void(*main)(),void(*exit)())
    {
        auto l4 = paging::table_alloc();
        auto& ol4 = process_array[execution_index].paging_root;
        l4->copy_from(*ol4,0,512);
        uint16_t stack_index = ((uint64_t)stack_top_address & (0x00000000001ff000UL<<27))>>(12+27);
        l4->set_entry(stack_index,nullptr,0);
        return create_process((void*)main,l4,process_array[execution_index].level,execution_index,exit);
    }

    void init_process_array()
    {
        for(uint64_t i = 0; i<MAX_PROCESS_NUMBER;i++)
        {
            process_array[i].id=i;
        }
    }

    void* create_stack(paging::page_table_t* paging_root,interrupt::privilege_level_t privilege_level)
    {
        if(privilege_level==interrupt::privilege_level_t::user)
        {
            uint16_t flags = paging::flags::RW | paging::flags::USER;
            auto ret = paging::map((void*)stack_top_address,stack_pages,flags,paging_root,[](void* virtual_address,bool big_pages){return paging::frame_alloc();},false);
            if(ret != (void*)0xffffffffffffffff)
                abort("Process user stack not created");
        }
        uint16_t sys_flags = paging::flags::RW;
        auto ret = paging::map((void*)system_stack_top_address,stack_pages,sys_flags,paging_root,[](void* virtual_address,bool big_pages){return paging::frame_alloc();},false);
        if(ret != (void*)0xffffffffffffffff)
            abort("Process system stack not created");
        return (void*)(privilege_level==interrupt::privilege_level_t::user?stack_bottom_address:system_stack_bottom_address);
    }
    void destroy_stack(paging::page_table_t* paging_root)
    {
        paging::unmap((void*)stack_top_address,stack_pages,paging_root,[](void* phisical_address,bool big_pages){return paging::free_frame(phisical_address);},false);
        paging::unmap((void*)system_stack_top_address,stack_pages,paging_root,[](void* phisical_address,bool big_pages){return paging::free_frame(phisical_address);},false);
    }

    uint64_t create_process(void* entrypoint,paging::page_table_t* paging_root,interrupt::privilege_level_t level,uint64_t father_id, void(*fin)())
    {
        if(process_count<MAX_PROCESS_NUMBER)
        {
            //auto translated_entry = paging::virtual_to_phisical(entrypoint,paging_root);//for debug
            auto last_trie = paging::get_current_trie();
            paging::set_current_trie(paging_root);

            auto stack_base = (void*)((uint64_t)create_stack(paging_root,level)-16);
            //interrupt::context_t* context = (interrupt::context_t*)((uint64_t)stack_base-sizeof(interrupt::context_t));
            interrupt::context_t context;
            context.ss = level==interrupt::privilege_level_t::user?interrupt::GDT_USER_DATA_SEGMENT:0;
            context.cs = level==interrupt::privilege_level_t::user?interrupt::GDT_USER_CODE_SEGMENT:interrupt::GDT_SYSTEM_CODE_SEGMENT;
            constexpr uint64_t IF = 0x0200;
            constexpr uint64_t IOPL = 0x3000;
            context.rflags = level==interrupt::privilege_level_t::user?IF:IF|IOPL;
            context.rip = (uint64_t)entrypoint;
            context.rsp = (uint64_t)stack_base;
            context.rbp = (uint64_t)stack_base;
            context.rax = context.rbx = context.rcx = context.rdx = 
                context.rsi = context.rdi =
                context.r8 = context.r9 = context.r10 = context.r11 = context.r12 = context.r13 = context.r14 = context.r15 = context.fs = context.gs = context.int_num = context.int_info = 0;

            //tricks the function to call automatically user::exit() it reaches the end (it can cause a crash in user code we need to fix this later)
            *(void**)((uint64_t)stack_base) = (void*)fin;
            

            uint64_t process_id = get_available_index();

            process_array[process_id].id = process_id;
            process_array[process_id].father_id = father_id;
            process_array[process_id].is_present = true;
            process_array[process_id].level = level;
            process_array[process_id].paging_root = paging_root;
            process_array[process_id].context = context;
            //process_array[process_id].data_pointer = stack_base;
            
            process_array[process_id].process_heap = heap{(void*)(level==interrupt::privilege_level_t::user?stack_top_address:system_stack_top_address),1024*1024};
            process_array[process_id].next = nullptr;

            paging::set_current_trie(last_trie);
            debug::log(debug::level::inf,"Process %uld created, level: %s",process_id,level==interrupt::privilege_level_t::user?"user":"system");
            process_count++;
            add_ready(process_id);
            return process_id;
        }
        else
            return MAX_PROCESS_NUMBER;
    }

    void destroy_semaphore(uint64_t id)
    {
        semaphore_array[id].is_present=false;
        while(semaphore_array[id].waiting_list_head)
        {
            add_ready(pop_from_list(semaphore_array[id].waiting_list_head,semaphore_array[id].waiting_list_tail));
        }
    }
    void destroy_process(uint64_t id)
    {
        if(process_array[id].is_present)
        {
            paging::set_current_trie(&paging::identity_l4_table);
            process_array[id].is_present = false;
            destroy_stack(process_array[id].paging_root);
            if(process_array[id].father_id==MAX_PROCESS_NUMBER)
            {
                paging::destroy_paging_trie(process_array[id].paging_root);
            }
            else
            {
                for(uint64_t i = 0;i<512;i++)
                {
                    process_array[id].paging_root->set_entry(i,nullptr,0);
                }
                paging::free_table(process_array[id].paging_root);
            }
            process_count--;
            for(uint64_t i=0;i<MAX_SEMAPHORE_NUMBER;i++)
            {
                if(semaphore_array[i].is_present && semaphore_array[i].creator_id==execution_index)
                {
                    destroy_semaphore(i);
                }
            }
            for(uint64_t i=0;i<MAX_PROCESS_NUMBER;i++)//kill the younglings (child processes)
            {
                if(process_array[i].is_present && process_array[i].father_id==id)
                {
                    destroy_process(i);
                }
            }
            //we should remove it from the lists but we don't care, just check if is present before executing
        }
    }

    volatile uint64_t scheduler_timer_ticks = 0;
    uint64_t next_present_process()
    {
        uint64_t id;
        do
        {
            id = pop_ready();
        }while(!process_array[id].is_present);
        return id;
    }
    void scheduler()
    {
        if(execution_index<MAX_PROCESS_NUMBER)
        {
            if(scheduler_timer_ticks>=timesharing_interval||(!process_array[execution_index].is_present))
            {
                next();
            }
        }
        else
        {
            execution_index = 0;//initialization
            drop();
        }
    }
    void drop()
    {
        scheduler_timer_ticks = 0;
        debug::log(debug::level::inf,"process dropped");
        execution_index = next_present_process();
    }
    void next()
    {
        scheduler_timer_ticks = 0;
        auto last_exec = execution_index;
        if(ready_queue)
        {
            do
            {
                execution_index = next_present_process();
                add_ready(last_exec);
            } while (ready_queue && execution_index == 0);
        }
    }


    
    void log_panic(const char* message)
    {
        auto context = &process_array[execution_index].context;
        debug::log(debug::level::err,"----------------------------------------");
        debug::log(debug::level::err,"Process %uld crashed",execution_index);
        if(context->int_num<32)
            debug::log(debug::level::err,"Cause: %s (0x%x)",interrupt::isr_messages[context->int_num],context->int_info);
        else if(context->int_num>=32&&context->int_num<32+24)
            debug::log(debug::level::err,"Cause: Error during IRQ number %uld",context->int_num-32);
        else if(context->int_num==0x80)
            debug::log(debug::level::err,"Cause: Error during syscall %s",syscalls_names[context->rdi]);
        else 
            debug::log(debug::level::err,"Cause: INT %uld (%uld)",context->int_num, context->int_info);
        debug::log(debug::level::err,"RIP: 0x%p",context->rip);
        if(context->int_num==14)
        {//page fault
            debug::log(debug::level::err,"Accessed address: 0x%p",paging::get_cr2());
        }
        debug::log(debug::level::err,"CS:0x%p SS:0x%p",context->cs,context->ss);
        debug::log(debug::level::err,"Stack Pointer: 0x%p",context->rsp);
        debug::log(debug::level::err,"Base pointer: 0x%p",context->rbp);
        debug::log(debug::level::err,"----------------------------------------");
    }
    void panic(const char* message)
    {
        auto context = &process_array[execution_index].context;
        clear(0x4f);
        printf("\033c\xf4KERNEL PANIC\n");
        if(message)
            printf("MSG: %s\n",message);
        printf("Process id: %uld\n",execution_index);
        printf("Cause: ");
        if(context->int_num<32)
            printf("%s (0x%x)",interrupt::isr_messages[context->int_num],context->int_info);
        else if(context->int_num>=32&&context->int_num<32+24)
            printf("Error during IRQ number %uld",context->int_num-32);
        else if(context->int_num==0x80)
            printf("Error during syscall %s",syscalls_names[context->rdi]);
        else 
            printf("INT %uld (%uld)",context->int_num, context->int_info);
        printf("\nRIP: 0x%p\n",context->rip);
        if(context->int_num==14)
        {//page fault
            printf("Accessed address: 0x%p\n",paging::get_cr2());
            printf("Error flags: ");
            if(context->int_info & (1<<0))
                printf("[Present] ");
            if(context->int_info & (1<<1))
                printf("[Write] ");
            if(context->int_info & (1<<2))
                printf("[User] ");
            if(context->int_info & (1<<3))
                printf("[Reserved write] ");
            if(context->int_info & (1<<4))
                printf("[Instruction fetch]");
            printf("\n");
        }
        printf("\nFlags: 0b%b\nCS: 0x%p\n",context->rflags,context->cs);
        printf("RAX:0x%p RBX:0x%p\nRCX:0x%p RDX:0x%p\nRSI:0x%p RDI:0x%p\n",context->rax,context->rbx,context->rcx,context->rdx,context->rsi,context->rdi);
        printf("Stack segment: 0x%p\nStack Pointer: 0x%p\nBase pointer: 0x%p\n",context->ss,context->rsp,context->rbp);

        put_char(':',0x4f,78,24);
        put_char('(',0x4f,79,24);
        asm volatile("hlt");
    }

    
    void abort(const char* msg)
    {
        if(process_array[execution_index].level==interrupt::privilege_level_t::system || force_panic)
        {
            debug::log(debug::level::err,"KERNEL PANIC");
            panic(msg);
        }
        else
        {
            /*if(process_array[execution_index].context->int_num<interrupt::ISR_SIZE)
                printf("\033c\x0cProcess %ld aborted, reason: %s ", execution_index, interrupt::isr_messages[process_array[execution_index].context->int_num]);
            else
                printf("\033c\x0cProcess %ld aborted ", execution_index);
            if(msg)
                printf("message: %s",msg);
            printf("\n");*/
            log_panic(msg);
            destroy_process(execution_index);
        }
    }
    void save_state(interrupt::context_t* context)
    {
        //current process is execution_index if it's valid
        if(execution_index<MAX_PROCESS_NUMBER)
        {//save the current state to the process descriptor
            process_array[execution_index].context = *context;
        }
    }
    interrupt::context_t* load_state()
    {
        //we need to call the scheduler to update execution_index
        scheduler();
        //once we know which process to run, update cr3 and stack pointer, the rest of the context
        //will be automatically restored before iret
        paging::set_current_trie(process_array[execution_index].paging_root);//change cr3

        return &process_array[execution_index].context;
    }

};