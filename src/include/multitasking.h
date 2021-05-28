#pragma once
#include "interrupt.h"
#include "paging.h"
#include "heap.h"

namespace multitasking
{
    struct process_descriptor_t
    {
        bool is_present = false;
        bool is_ready = false;
        uint64_t id;
        interrupt::privilege_level_t level;
        //uint64_t priority;
        void* data_pointer;
        interrupt::context_t* context;
        paging::page_table_t* paging_root;//cr3 register

        //process_descriptor_t* next;
    };

    extern uint64_t scheduler_timer_ticks;
    constexpr uint64_t timesharing_interval = 50;


    constexpr uint64_t MAX_PROCESS_NUMBER = 1024;
    extern process_descriptor_t process_array[MAX_PROCESS_NUMBER];
    extern uint64_t execution_index;
    extern uint64_t process_count;
    extern uint64_t max_process_index;

    uint64_t get_available_index();

    void init_process_array();

    void* create_stack(paging::page_table_t* paging_root);
    void destroy_stack(paging::page_table_t* paging_root);

    uint64_t create_process(void* entrypoint,paging::page_table_t* paging_root,interrupt::privilege_level_t level);
    void destroy_process(uint64_t id);

    //updates execution_index with the index of the next process to run
    void scheduler();

    //the current process caused a panic, system halted
    void panic();
    //kills the current process because an error occurred
    void abort();
    //saves context about the current running process
    void save_state(interrupt::context_t* context);
    //runs the scheduler, changes cr3 and returns the stack index
    interrupt::context_t* load_state();

};

