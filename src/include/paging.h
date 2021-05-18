#pragma once
#include <stdint.h>
namespace paging
{
    struct page_table_t
    {
        uint32_t data[512];
        void set_entry(uint16_t index,void* child, uint16_t flags);
        bool is_present(uint16_t index);
    };

    extern "C" page_table_t identity_l4_table;
};