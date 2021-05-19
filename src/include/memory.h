#pragma once
#include <stddef.h>
#include <stdint.h>
#include "heap.h"


namespace memory
{
    extern "C"
    {
        extern uint32_t _end;//it's address is the end of kernel program in memory
        void memcpy(const void* dst, const void* src, size_t size);
        void memset(void* addr,size_t size,uint8_t fill=0);
    
        void memcpyl(const void* dst, const void* src, size_t long_size);
        void memsetl(void* addr,size_t long_size,uint32_t fill=0);
    }
    
    void memorl(const void* dst, const void* src, size_t long_size);
    void memxorl(const void* dst, const void* src, size_t long_size);
    void memandl(const void* dst, const void* src, size_t long_size);
    
    bool memcmp(const void* dst, const void* src, size_t size);
    
    void* align(void* address,uint64_t align);
};