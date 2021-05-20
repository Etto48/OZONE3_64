#pragma once
#include <stdint.h>
#include "memory.h"
#include "string_tools.h"
#include "printing.h"

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
        uint64_t data[512];
        void set_entry(uint16_t index,void* child, uint16_t flags);
        void copy_from(const page_table_t& source_table,uint16_t from, uint16_t count);
        bool is_present(uint16_t index);
        page_table_t* operator[](uint16_t index);
    };

    extern "C" page_table_t identity_l4_table;

    union frame_descriptor_t
    {
        uint64_t present_entries;//NOT FREE: number of present entries if the frame holds a page_table_t
        uint64_t next_free_frame_index;//FREE: index of the next free frame
    };

    constexpr uint64_t FRAME_COUNT = 0x40000 * 8;//1GiB * GiB count
    extern frame_descriptor_t frame_descriptors[FRAME_COUNT];
    extern uint64_t first_free_frame_index;
    extern uint64_t kernel_frames;
    extern uint64_t secondary_frames;
    extern volatile uint64_t free_frames;

    void init_frame_mapping();

    void* get_frame_address(uint64_t frame_descriptor_index);
    uint64_t get_frame_index(void* frame_phisical_address);

    //extract a free frame from the list of free frames
    void* frame_alloc();
    //inserts a frame into the list of free frames
    void free_frame(void* address);

    //creates an empty table taking a free frame
    page_table_t* table_alloc();
    //frees a table returning the frame
    void free_table(page_table_t* address);
    
    class phisical_address_generator_t
    {
    public:
        virtual void* operator()(void* virtual_address, bool big_pages) = 0;    
    };

    
    //should be 0xffffffffffffffff if it's all ok, if you're trying to map a region that's already mapped
    //the function returns the first address that generated the error
    template <typename T>
    void* map(void* virtual_address, uint64_t page_count, uint16_t flags, page_table_t* trie_root,T gen_addr, bool big_pages = false);
    void* unmap(void* virtual_address, uint64_t page_count, page_table_t* trie_root, bool big_pages = false);


    void* extend_identity_mapping();
};