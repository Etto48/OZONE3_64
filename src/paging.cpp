#include "include/paging.h"

namespace paging
{
    void page_table_t::set_entry(uint16_t index,void* child, uint16_t flags)
    {
        flags = 0b111111111111 & flags;
        uint32_t final_data = (uint32_t)(uint64_t)child & (~0b111111111111);
        final_data |= flags;
        if(index<512)
        {
            this->data[index] = final_data;
        }
    }

    bool page_table_t::is_present(uint16_t index)
    {
        if(index<512)
        {
            return data[index]&1;
        }
        return false;
    }
};