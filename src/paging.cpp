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
            return data[index]&flags::PRESENT;
        }
        return false;
    }

    frame_descriptor_t frame_descriptors[FRAME_COUNT];
    uint64_t first_free_frame = FRAME_COUNT;
    uint64_t kernel_frames = 0;
    uint64_t secondary_frames = 0;

    void init_frame_mapping()
    {
        /* _____________
        * |KERNEL FRAMES|
        * |             |
        * |_____________|
        * |SECON. FRAMES|
        * |             |
        * |             |
        * |_ _ _ _ _ _ _|
        */

        void* first_secondary_frame = memory::align((void*)&memory::_end,0x1000);
        kernel_frames = get_frame_index(first_secondary_frame);//frames occupied by the kernel module
        secondary_frames = FRAME_COUNT-kernel_frames;//frames free at the start of the system

        first_free_frame = kernel_frames;//it's the index of the first secondary_frame
        for(uint64_t i=first_free_frame+1; i<FRAME_COUNT;i++)
        {
            frame_descriptors[i-1].next_free_frame = i;
        }
        //find filled frames and page tables
        uint64_t identity_l4_table_frame_index = get_frame_index(&identity_l4_table);
        for(uint64_t i=0;i<first_free_frame;i++)
        {
            
            if(i==identity_l4_table_frame_index)//l4 table 1 entry
            {
                frame_descriptors[i].present_entries=1;
            }
            else if(i==identity_l4_table_frame_index+1)//l3 table 1 entry
            {
                frame_descriptors[i].present_entries=1;
            }
            else if(i==identity_l4_table_frame_index+2)//l2 table 512 entries
            {
                frame_descriptors[i].present_entries=512;
            }
            else
            {
                frame_descriptors[i].present_entries=0;
            }
        }
    }

    void* get_frame_address(uint64_t frame_descriptor_index)
    {
        return (void*)(frame_descriptor_index*0x1000);
    }
    uint64_t get_frame_index(void* frame_phisical_address)
    {
        return ((uint64_t)frame_phisical_address/0x1000);
    }

    bool map(void* virtual_address, uint64_t page_count, uint16_t flags, page_table_t* trie_root, bool big_pages)
    {
        if((uint64_t)virtual_address&0xffffffff00000fff)//check align 0x1000
            return false;
        //0b0000 0000 0000 0000  1111 1111 1000 0000   0000 0000 0000 0000  0000 0000 0000 0000 l4
        //0b0000 0000 0000 0000  0000 0000 0111 1111   1100 0000 0000 0000  0000 0000 0000 0000 l3
        //0b0000 0000 0000 0000  0000 0000 0000 0000   0011 1111 1110 0000  0000 0000 0000 0000 l2
        //0b0000 0000 0000 0000  0000 0000 0000 0000   0000 0000 0001 1111  1111 0000 0000 0000 l1
        //0b1111 1111 1111 1111  1111 1111 1111 1111   1111 1111 1111 1111  1111 1111 1111 1111
        uint16_t l4_index = ((uint64_t)virtual_address & (0x00000000001ff000UL<<27))>>(12+27);
        uint16_t l3_index = ((uint64_t)virtual_address & (0x00000000001ff000UL<<18))>>(12+18);
        uint16_t l2_index = ((uint64_t)virtual_address & (0x00000000001ff000UL<<9))>>(12+9);
        uint16_t l1_index = ((uint64_t)virtual_address & (0x00000000001ff000UL<<0))>>(12);
        for(uint64_t i = 0; i<page_count; i++)
        {
            uint16_t l4_loop_index = l4_index+i/(512*512*512);
            uint16_t l3_loop_index = (l3_index+i/(512*512))%512;
            uint16_t l2_loop_index = (l2_index+i/(512))%512;
            uint16_t l1_loop_index = (l1_index+i)%512;
            //if(!trie_root->is_present(l4_loop_index))
            //    trie_root->data[l4_loop_index] = kmalloc(sizeof(page_table_t),0x1000);
            //WIP
        }
        return true;
    }
};