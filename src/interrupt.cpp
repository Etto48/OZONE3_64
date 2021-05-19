#include "include/interrupt.h"

namespace interrupt
{

    extern "C"
    {
    	void isr0();void isr1();void isr2();void isr3();void isr4();void isr5();void isr6();void isr7();
    	void isr8();void isr9();void isr10();void isr11();void isr12();void isr13();void isr14();void isr15();
    	void isr16();void isr17();void isr18();void isr19();void isr20();void isr21();void isr22();void isr23();
    	void isr24();void isr25();void isr26();void isr27();void isr28();void isr29();void isr30();void isr31();
    
        void irq0();void irq1();void irq2();void irq3();void irq4();void irq5();void irq6();void irq7();
        void irq8();void irq9();void irq10();void irq11();void irq12();void irq13();void irq14();void irq15();
        void irq16();void irq17();void irq18();void irq19();void irq20();void irq21();void irq22();void irq23();
    }

    constexpr uint16_t GDT_SYSTEM_CODE_SEGMENT = 0x08;
    constexpr uint16_t GDT_USER_CODE_SEGMENT = 0x10;
    constexpr uint16_t DPL_SYSTEM = 0;
    constexpr uint64_t DPL_USER = 0b01100000;

    idt_entry_t::idt_entry_t(void* offset,privilege_level_t interrupt_level,privilege_level_t callee_level,idt_gate_type_t type,bool is_present)
    {
        uint64_t _offset = (uint64_t)offset;
        offset_low = _offset&0xffff;
        offset_middle = (_offset&0xffff0000)>>16;
        offset_high = (_offset&0xffffffff00000000)>>32;
        switch (interrupt_level)
        {
        case privilege_level_t::system:
            code_segment_selector = GDT_SYSTEM_CODE_SEGMENT;
            break;
        case privilege_level_t::user:
            code_segment_selector = GDT_USER_CODE_SEGMENT;
            break;
        }
        switch (callee_level)
        {
        case privilege_level_t::system:
            type_and_attributes = DPL_SYSTEM;
            break;
        case privilege_level_t::user:
            type_and_attributes = DPL_USER;
            break;
        }

        if(is_present)
            type_and_attributes|=0b10000000;
        switch (type)
        {
        case idt_gate_type_t::interrupt_gate:
            type_and_attributes|=0xe;   
            break;
        case idt_gate_type_t::trap_gate:
            type_and_attributes|=0xf;
            break;
        }

        ist = 0;
        zero = 0;
    }
    idtr_t::idtr_t(idt_entry_t* idt,uint16_t entries)
    {
        offset = (uint64_t)&idt;
        limit = sizeof(idt_entry_t)*entries-1;
    }
    idtr_t IDTR(IDT,IDT_SIZE);
    idt_entry_t IDT[IDT_SIZE];


    extern "C" void isr_handler()
    {

    }
    extern "C" void irq_handler()
    {

    }
    extern "C" void unknown_interrupt()
    {

    }
};