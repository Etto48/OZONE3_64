#pragma once
#include <stdint.h>

class heap
{
private:
    void* address;
public:
    heap(void* address, uint64_t size);
    void* malloc(uint64_t size);
    void* malloc(uint64_t size, uint64_t align);
    void free(void* loc);
};
