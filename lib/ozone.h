#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

namespace user
{

    constexpr uint64_t INVALID_PROCESS = 1024;
    constexpr uint64_t INVALID_SEMAPHORE = 1024;

    uint64_t sys_call_n(uint64_t sys_call_number,uint64_t arg0 = 0, uint64_t arg1 = 0);

    //returns the id of the current process
    uint64_t get_id();
    //stops the process for <ticks> ticks of clock
    void sleep(uint64_t ticks);
    //kills the current process
    void exit(uint64_t ret = 0);
    //used to return from a process
    extern "C" void fin();
    //allocates a semaphore to manage access to shared resources
    uint64_t create_semaphore(uint64_t start_count);
    //request access to a shared resource
    void acquire_semaphore(uint64_t semaphore_id);
    //releases the access to a shared resource 
    void release_semaphore(uint64_t semaphore_id);
    //creates a child process which inherits the pagigng trie (stack excluded) and has entrypoint main
    //returns the process_id of the new process, user::exit() will be automatically called if the functions returns
    template <typename T>
    uint64_t fork(T(*main)())
    {//we should pass the address of user::exit()
        return sys_call_n(6,(uint64_t)main,(uint64_t)user::fin);
    }
    //waits for a process to end and returns its return
    uint64_t join(uint64_t id);
    //calls a function from a driver returns the process id of the call, it's asyncronous so you can make a "join" call later to take the return
    uint64_t driver_call(uint64_t driver_number, uint64_t function_number);
};
void* operator new(size_t size);
//void* operator new(size_t size, size_t align);
void operator delete(void* address);
void operator delete(void* address, unsigned long);

namespace system
{
    uint64_t sys_call_n(uint64_t sys_call_number,uint64_t arg0 = 0,uint64_t arg1 = 0,uint64_t arg2 = 0,uint64_t arg3 = 0);

    //sets the selected process as the driver for the selected irq
    void set_driver(uint64_t irq_number,uint64_t process_id);
    //waits for the next interrupt associated with the process, returns the id of the interrupted process
    uint64_t wait_for_interrupt();

    void set_driver_function(uint64_t irq_number,uint64_t function_id, uint64_t(*function)());
};
