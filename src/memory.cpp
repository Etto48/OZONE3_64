#include "include/memory.h"

namespace memory
{
    void* align(void* address,uint64_t align)
    {
        uint64_t _addr = (uint64_t)address;
        _addr = (_addr % align == 0 ? _addr : ((_addr + align - 1) / align) * align);
        return (void*)_addr;
    }   
};