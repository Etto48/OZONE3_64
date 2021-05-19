#pragma once
#include <stdint.h>
#include "memory.h"
#include "string_tools.h"

namespace paging
{
    namespace flags
    {
        constexpr uint16_t PRESENT =        0b000000000001;
        constexpr uint16_t RW =             0b000000000010;
        constexpr uint16_t USER =           0b000000000100;
        constexpr uint16_t WRITE_THROUGH =  0b000000001000;
        constexpr uint16_t DISABLE_CACHE =  0b000000010000;
        constexpr uint16_t ACCESSED =       0b000000100000;
        constexpr uint16_t BIG =            0b000001000000;
    };
    struct page_table_t
    {
        uint32_t data[512];
        void set_entry(uint16_t index,void* child, uint16_t flags);
        bool is_present(uint16_t index);
    };

    extern "C" page_table_t identity_l4_table;

    union frame_descriptor_t
    {
        uint64_t present_entries;//NOT FREE: number of present entries if the frame holds a page_table_t
        uint64_t next_free_frame;//FREE: index of the next free frame
    };

    constexpr uint64_t FRAME_COUNT = 0x200000;
    extern frame_descriptor_t frame_descriptors[FRAME_COUNT];
    extern uint64_t first_free_frame;
    extern uint64_t kernel_frames;
    extern uint64_t secondary_frames;

    void init_frame_mapping();

    void* get_frame_address(uint64_t frame_descriptor_index);
    uint64_t get_frame_index(void* frame_phisical_address);
    
    bool map(void* virtual_address, uint64_t page_count, uint16_t flags, page_table_t* trie_root, bool big_pages = false);
};