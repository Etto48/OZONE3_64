#pragma once
#include <stdint.h>
#include <stddef.h>
#include "memory.h"

class heap
{
private:
    void* address;
    bool is_empty;
public:
    heap(void* address);
    void* malloc(uint64_t size);
    void* malloc(uint64_t size, uint64_t align);
    void free(void* loc);
};

extern heap system_heap;