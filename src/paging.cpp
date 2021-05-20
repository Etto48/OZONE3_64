#include "include/paging.h"

namespace paging
{
    void page_table_t::set_entry(uint16_t index,void* child, uint16_t flags)
    {
        auto old_data = data[index];
        flags = 0xfff & flags;
        uint64_t final_data = (uint64_t)child & ((0x0000fffffffff000));
        final_data |= flags;
        if(index<512)
        {
            auto frame_index = get_frame_index(this);
            if((final_data&flags::PRESENT) && !(old_data&flags::PRESENT))
            {
                frame_descriptors[frame_index].present_entries++;
            }
            else if(!(final_data&flags::PRESENT) && (old_data&flags::PRESENT))
            {
                frame_descriptors[frame_index].present_entries--;
            }
            this->data[index] = final_data;
        }
    }
    void page_table_t::copy_from(const page_table_t& source_table,uint16_t from, uint16_t count)
    {
        for(uint32_t i=from; i<from+count;i++)
        {
            set_entry(i,(void*)source_table.data[i],(uint16_t)source_table.data[i]);
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
    page_table_t* page_table_t::operator[](uint16_t index)
    {
        if(index<512)
            return (page_table_t*)(data[index] & (~0xfff));
        else
            return nullptr;
    }

    frame_descriptor_t frame_descriptors[FRAME_COUNT];
    uint64_t first_free_frame_index = FRAME_COUNT;
    uint64_t kernel_frames = 0;
    uint64_t secondary_frames = 0;
    volatile uint64_t free_frames = 0;

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
        free_frames = secondary_frames;

        first_free_frame_index = kernel_frames;//it's the index of the first secondary_frame
        for(uint64_t i=first_free_frame_index+1; i<=FRAME_COUNT;i++)
        {
            frame_descriptors[i-1].next_free_frame_index = i;
        }
        //find filled frames and page tables
        uint64_t identity_l4_table_frame_index = get_frame_index(&identity_l4_table);
        for(uint64_t i=0;i<first_free_frame_index;i++)
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


    void* frame_alloc()
    {
        auto fff_index = first_free_frame_index;
        if(fff_index>=FRAME_COUNT)
            return nullptr;//ERROR no more frames
        first_free_frame_index = frame_descriptors[fff_index].next_free_frame_index;
        frame_descriptors[fff_index].next_free_frame_index=0;
        free_frames--;
        return get_frame_address(fff_index);
    }
    void free_frame(void* address)
    {
        auto new_frame_index = get_frame_index(address);
        if(new_frame_index<kernel_frames)
            return;//ERROR you cannot free a kernel frame
        frame_descriptors[new_frame_index].next_free_frame_index = first_free_frame_index;
        first_free_frame_index = new_frame_index;
        free_frames++;
    }
    
    page_table_t* table_alloc()
    {
        auto new_table_address = frame_alloc();
        if(new_table_address)
        {
            auto frame_index = get_frame_index(new_table_address);
            memory::memset(new_table_address,0,sizeof(page_table_t));
            frame_descriptors[frame_index].present_entries=0;
        }
        return (page_table_t*)new_table_address;
    }
    void free_table(page_table_t* address)
    {
        auto frame_index = get_frame_index(address);
        if(frame_descriptors[frame_index].present_entries!=0)
            return; //ERROR you must empty the table first
        free_frame(address);
    }


    template <typename T>
    void* map(void* virtual_address, uint64_t page_count, uint16_t flags, page_table_t* trie_root,T gen_addr, bool big_pages)
    {
        if(!memory::is_normalized(virtual_address))
            return nullptr;
        if((uint64_t)virtual_address&0x0000000000000fff)//check align 0x1000
            return nullptr;
        if((uint64_t)virtual_address&0x00000000001fffff && big_pages)
            return nullptr;
        virtual_address = (void*)((uint64_t)virtual_address &(0x0000fffffffff000));

        //0b0000 0000 0000 0000  1111 1111 1000 0000   0000 0000 0000 0000  0000 0000 0000 0000 l4
        //0b0000 0000 0000 0000  0000 0000 0111 1111   1100 0000 0000 0000  0000 0000 0000 0000 l3
        //0b0000 0000 0000 0000  0000 0000 0000 0000   0011 1111 1110 0000  0000 0000 0000 0000 l2
        //0b0000 0000 0000 0000  0000 0000 0000 0000   0000 0000 0001 1111  1111 0000 0000 0000 l1
        //0b1111 1111 1111 1111  1111 1111 1111 1111   1111 1111 1111 1111  1111 1111 1111 1111
        uint16_t l4_index = ((uint64_t)virtual_address & (0x00000000001ff000UL<<27))>>(12+27);
        uint16_t l3_index = ((uint64_t)virtual_address & (0x00000000001ff000UL<<18))>>(12+18);
        uint16_t l2_index = ((uint64_t)virtual_address & (0x00000000001ff000UL<<9))>>(12+9);
        uint16_t l1_index = ((uint64_t)virtual_address & (0x00000000001ff000UL<<0))>>(12);

        uint16_t high_level_flags = (flags & (flags::RW|flags::USER)) | flags::PRESENT;
        uint16_t low_level_flags = (flags & (flags::RW|flags::USER|flags::WRITE_THROUGH|flags::DISABLE_CACHE)) | flags::PRESENT;

        for(uint64_t i = 0; i<page_count; i++)
        {
            uint16_t l4_loop_index = l4_index+i/(512*512*512);
            uint16_t l3_loop_index = (l3_index+i/(512*512))%512;
            uint16_t l2_loop_index = (l2_index+i/(512))%512;
            uint16_t l1_loop_index = (l1_index+i)%512;
            
            if(big_pages)
            {
                l4_loop_index = l4_index+i/(512*512);
                l3_loop_index = (l3_index+i/(512))%512;
                l2_loop_index = (l2_index+i)%512;
                l1_loop_index = 0;
            }

            void* current_addr = memory::normalize((void*)((uint64_t)l4_loop_index<<(12+27)| (uint64_t)l3_loop_index<<(12+18) | (uint64_t)l2_loop_index<<(12+9) | (uint64_t)l1_loop_index<<12));

            void* phisical_address = gen_addr(current_addr,big_pages);
            if(!phisical_address)
                return current_addr;

            if(!trie_root->is_present(l4_loop_index))
            {//you need to allocate a l3 page
                auto new_l3 = table_alloc();
                trie_root->set_entry(l4_loop_index,new_l3,high_level_flags);
            }
            auto l3_table = (*trie_root)[l4_loop_index];
            if(!l3_table->is_present(l3_loop_index))
            {//you need to allocate a l2 page
                auto new_l2 = table_alloc();
                l3_table->set_entry(l3_loop_index,new_l2,high_level_flags);
            }
            auto l2_table = (*l3_table)[l3_loop_index];
            if(!l2_table->is_present(l2_loop_index))
            {
                if(!big_pages)//you need to allocate a l1 page
                {
                    auto new_l1 = table_alloc();
                    l2_table->set_entry(l2_loop_index,new_l1,high_level_flags);
                }
                else
                {
                    void* new_frame = phisical_address;
                    l2_table->set_entry(l2_loop_index,new_frame,low_level_flags|flags::BIG);
                    continue;
                }
            }
            else if(l2_table->data[l2_loop_index]&flags::BIG || big_pages)//if a big page is allocated error
            {
                return current_addr;
            }
            auto l1_table = (*l2_table)[l2_loop_index];
            if(l1_table->is_present(l1_loop_index))
                return current_addr;
            void* new_frame = phisical_address;
            l1_table->set_entry(l1_loop_index,new_frame,low_level_flags);
        }
        return (void*)0xffffffffffffffff;
    }
    void* unmap(void* virtual_address, uint64_t page_count, page_table_t* trie_root, bool big_pages)
    {
        //WIP
        return (void*)0x0;
    }

    void* extend_identity_mapping()
    {
        return map((void*)0x40000000,FRAME_COUNT*0x1000/0x200000,flags::RW,&identity_l4_table,[](void* v_address,bool big_pages){return v_address;},true);
    }
};