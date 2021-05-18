.set ALIGN,    1<<0             /* align loaded modules on page boundaries */
.set MEMINFO,  1<<1             /* provide memory map */
.set FLAGS,    ALIGN | MEMINFO  /* this is the Multiboot 'flag' field */
.set MAGIC,    0x1BADB002       /* 'magic number' lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */

.align 16
.section .bss
stack_bottom:
	.space 0x4000, 0
stack_top:

.section .text
.global _start
#PAGES
#0x1000 PML4T
#0x2000 PDPT
#0x3000 PDT
#0x4000 PT
.align 4
multiboot:
.long   MAGIC 			
.long   FLAGS   				
.long   CHECKSUM
.code16
_start:
    #CLEAR PAGES
    mov $0x1000, %edi
    mov %edi, %cr3
    xor %eax, %eax
    mov $4096, %ecx
    rep stosl
    mov %cr3, %edi
    #FILL PAGES FOR FIRST 2MB
    movl $0x2003, (%edi)
    add $0x1000, %edi
    movl $0x3003, (%edi)
    add $0x1000, %edi
    movl $0x4003, (%edi)
    add $0x1000, %edi
    
    mov $0x00000003, %ebx
    mov $512, %ecx
    .set_entry:
        mov %ebx, (%edi)
        add $0x1000, %ebx
        add $0x8, %edi
        loop .set_entry
    mov %cr4, %eax
    or $(1<<4), %eax
    mov %eax, %cr4
    #SWITCH FROM REAL MODE
    mov $0xC0000080, %ecx #efer msr
    rdmsr
    or $0x00000100, %eax #efer.lme
    wrmsr
    mov %cr0, %eax
    or $0x80000000, %eax #cr0.pg
    mov %eax, %cr0
    #ENTER 64b SUBMODE
    lgdt GDT64.pointer #GDT
    hlt
    ljmp $0x8, $realm64

.code64
realm64:
    cli
    mov $0, %ax
    mov %ax, %ds                    # Set the data segment to the A-register.
    mov %ax, %es                    # Set the extra segment to the A-register.
    mov %ax, %fs                    # Set the F-segment to the A-register.
    mov %ax, %gs                    # Set the G-segment to the A-register.
    mov %ax, %ss                    # Set the stack segment to the A-register.
    mov $0xb8000, %edi
    mov $0x1F201F201F201F20, %rax
    mov $500, %ecx
    rep stosq
    hlt
.code32


.section .data
.balign 4096
GDT64:                           # Global Descriptor Table (64-bit).
    GDT64.null:
    .set GDT64_NULL_OFFSET, . - GDT64   # The null descriptor.
    .word 0                         # Limit (low).
    .word 0                         # Base (low).
    .byte 0                         # Base (middle)
    .byte 0                         # Access.
    .byte 0                         # Granularity.
    .byte 0                         # Base (high).
    GDT64.code:
    /*.set GDT64_CODE_OFFSET, . - GDT64   # The code descriptor.
    .word 0                         # Limit (low).
    .word 0                         # Base (low).
    .byte 0                         # Base (middle)
    .byte 0b10011010                 # Access (exec/read).
    .byte 0b10101111                 # Granularity, 64 bits flag, limit19:16.
    .byte 0                         # Base (high).*/
    .quad 0x209c0000000000
    GDT64.data:
    .set GDT64_DATA_OFFSET, . - GDT64   # The data descriptor.
    .word 0                         # Limit (low).
    .word 0                         # Base (low).
    .byte 0                         # Base (middle)
    .byte 0b10010010                 # Access (read/write).
    .byte 0b00000000                 # Granularity.
    .byte 0                         # Base (high).
    GDT64.pointer:                    # The GDT-pointer.
    .word . - GDT64 - 1             # Limit.
    .quad GDT64                     # Base.

