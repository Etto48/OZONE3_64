#pragma once
#include <stdint.h>

namespace interrupt
{
    enum class privilege_level_t
    {
        system, //0x08 GDT OFFSET
        user    //0x10 GDT OFFSET
    };
    enum class idt_gate_type_t
    {
        interrupt_gate,
        trap_gate
    };
    struct idt_entry_t
    {
        uint16_t offset_low;
        uint16_t code_segment_selector;
        uint8_t ist;
        uint8_t type_and_attributes;
        uint16_t offset_middle;
        uint32_t offset_high;
        uint32_t zero;
        idt_entry_t(
            void* offset=nullptr,
            privilege_level_t interrupt_level=privilege_level_t::system,
            privilege_level_t callee_level=privilege_level_t::system,
            idt_gate_type_t type=idt_gate_type_t::interrupt_gate,
            bool is_present=false);
    } __attribute__((packed));

    struct idtr_t
    {
        uint16_t limit;
        uint64_t offset;
        idtr_t(idt_entry_t* idt,uint16_t entries);
    } __attribute__((packed));

    struct context_t
    {
        uint64_t rax, rbx, rcx, rdx,
            rsi, rdi,
            rsp, rbp,   
            r8, r9, r10, r11, r12, r13, r14, r15;
        uint16_t fs, gs;
    } __attribute__((packed));

    constexpr uint64_t IDT_SIZE = 256;
    constexpr uint64_t ISR_SIZE = 32;
    constexpr uint64_t IRQ_SIZE = 24;

    extern idtr_t IDTR;
    extern idt_entry_t IDT[IDT_SIZE];

    extern "C" void load_idt(idtr_t& idtr);
    extern "C" void isr_handler();
    extern "C" void irq_handler();
    extern "C" void unknown_interrupt();
    void init_interrupt();
};