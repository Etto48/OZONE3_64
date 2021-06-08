#pragma once
#include "interrupt.h"
#include "paging.h"
#include "heap.h"
#include <ozone.h>

namespace multitasking
{
    /*enum class process_priority_t
    {
        idle, normal
    };*/
    struct process_descriptor_t
    {
        bool is_present = false;
        uint64_t id;
        uint64_t father_id;
        interrupt::privilege_level_t level;
        //uint64_t priority;
        interrupt::context_t context;
        paging::page_table_t* paging_root;//cr3 register
        heap process_heap;

        process_descriptor_t* next = nullptr;
    };

    struct tss_t
    {
        uint32_t reserved0 : 4;
        uint64_t rsp0 : 8;
        uint64_t rsp1 : 8;
        uint64_t rsp2 : 8;
        uint64_t reserved1 : 8;
        uint64_t ist1 : 8;
        uint64_t ist2 : 8;
        uint64_t ist3 : 8;
        uint64_t ist4 : 8;
        uint64_t ist5 : 8;
        uint64_t ist6 : 8;
        uint64_t ist7 : 8;
        uint64_t reserved2 : 8;
        uint16_t reserved3 : 2;
        uint16_t iopb_offset : 2;
    } __attribute__((packed));

    struct semaphore_descriptor_t
    {
        bool is_present = false;
        int64_t count;

        uint64_t creator_id;

        process_descriptor_t* waiting_list_head = nullptr;
        process_descriptor_t* waiting_list_tail = nullptr;
    }; 

    extern volatile uint64_t scheduler_timer_ticks;
    constexpr uint64_t timesharing_interval = 10;

    constexpr uint64_t stack_pages = 256;
    constexpr uint64_t stack_bottom_address = 0xffffffffffffffff;
    constexpr uint64_t stack_top_address = stack_bottom_address - (stack_pages*0x1000-1);

    constexpr uint64_t system_stack_bottom_address = 0xfffffffffaffffff;
    constexpr uint64_t system_stack_top_address = system_stack_bottom_address - (stack_pages*0x1000-1);

    constexpr uint64_t MAX_PROCESS_NUMBER = 1024;
    constexpr uint64_t MAX_SEMAPHORE_NUMBER = 1024;
    constexpr bool force_panic = false;
    extern process_descriptor_t process_array[MAX_PROCESS_NUMBER];
    extern semaphore_descriptor_t semaphore_array[MAX_SEMAPHORE_NUMBER];
    extern volatile uint64_t execution_index;
    extern volatile uint64_t process_count;

    extern process_descriptor_t* ready_queue;
    extern process_descriptor_t* last_ready;

    extern const char* syscalls_names[];

    extern "C" tss_t tss;

    extern "C" uint64_t sys_stack_location;

    uint64_t get_available_index();
    uint64_t get_available_semaphore();

    uint64_t create_semaphore(int64_t starting_count);
    void acquire_semaphore(uint64_t semaphore_id);
    void release_semaphore(uint64_t semaphore_id);

    uint64_t fork(void(*main)(),void(*exit)());

    void init_process_array();

    uint64_t pop_ready();
    void add_ready(uint64_t process_id);

    void* create_stack(paging::page_table_t* paging_root);
    void destroy_stack(paging::page_table_t* paging_root);

    uint64_t create_process(void* entrypoint,paging::page_table_t* paging_root,interrupt::privilege_level_t level,uint64_t father_id, void(*fin)() = user::exit);
    void destroy_process(uint64_t id);

    //returns true if the selected memory space is accessible by the process, false if it's not
    bool is_process_memory(void* start, size_t len, uint64_t id);

    //updates execution_index with the index of the next process to run
    void scheduler();
    //if the current process is no more ready for some reason, a call to this function will exclude if from scheduling
    void drop();
    //executes the next process if there is one
    void next();


    //the current process caused a panic, system halted
    void panic(const char* message = nullptr);
    //kills the current process because an error occurred
    void abort(const char* msg = nullptr);
    //saves context about the current running process
    void save_state(interrupt::context_t* context);
    //runs the scheduler, changes cr3 and returns the stack index
    interrupt::context_t* load_state();

};
#include "clock.h"
#include "debug.h"
