.global _start
.code32
_start:
    jmp multiboot_entry

.align  4
multiboot_header:
     .long   0x1BADB002
     .long   0x3
     .long   -(0x1BADB002 + 0x3)
     .long   multiboot_header
     .long   _start
     .long   _edata
     .long   _end
     .long   multiboot_entry

multiboot_entry:

    lea stack_top, %esp
    
    call save_mbi
    //CHECK LONG MODE
    call check_multiboot
    call check_cpuid
    call check_long_mode

    //ENABLE PAGING
    call setup_page_tables
    call enable_paging

    //ENTER 64BIT LOADING GDT
    lgdt gdt64.pointer
    ljmp $8, $long_mode_start

save_mbi:
    mov %ebx, %esi
    lea mbi, %edi
    mov $(120/4), %ecx
    rep movsl
    ret

check_long_mode:
    mov $0x80000000, %eax
    cpuid
    cmp $0x80000001, %eax
    jb .no_long_mode

    mov $0x80000001, %eax
    cpuid
    test $(1<<29), %edx
    jz .no_long_mode

    ret
    .no_long_mode:
        mov $0x4c, %al #L
        jmp error

check_cpuid:
    pushfl
    pop %eax
    mov %eax, %eax
    xor $(1<<21), %eax
    push %eax
    popfl
    pushfl
    pop %eax
    push %ecx
    popfl
    cmp %eax, %ecx
    je .no_cpuid
    ret
    .no_cpuid:
        mov $0x43, %al #C
        jmp error

check_multiboot:
    cmp $0x2BADB002, %eax
    jne .no_multiboot
    ret
    .no_multiboot:
        mov $0x4d, %al #M
        jmp error  

setup_page_tables:
    lea page_table_l3, %eax
    or $0b11, %eax #present, writable
    mov %eax, page_table_l4

    lea page_table_l2, %eax
    or $0b11, %eax #present, writable
    mov %eax, page_table_l3

    mov $0x0, %ecx #counter for table
    .fill_loop:

        mov $0x200000, %eax #2MiB
        mul %ecx
        or $0b10000011, %eax #present, writable, huge_page
        mov %eax, page_table_l2(,%ecx,8)

        inc %ecx
        cmp $512, %ecx
        jne .fill_loop
    ret

enable_paging:
    //pass trie location
    lea page_table_l4, %eax
    mov %eax, %cr3

    //enable pae
    mov %cr4, %eax
    or $(1<<5), %eax
    mov %eax, %cr4

    //enable long mode
    mov $0xC0000080, %ecx
    rdmsr
    or $(1<<8), %eax
    wrmsr

    //enable paging
    mov %cr0, %eax
    or $(1<<31), %eax
    mov %eax, %cr0

    ret 

error:
    movl $0x4f524f45, 0xb8000
    movl $0x4f3a4f52, 0xb8004
    movl $0x4f204f20, 0xb8008
    mov %al, 0xb800a
    hlt
    
.section .bss
.align 0x1000
.global identity_l4_table
identity_l4_table:
page_table_l4:
    .skip 0x1000
page_table_l3:
    .skip 0x1000
page_table_l2:
    .skip 0x1000
.global stack_bottom
stack_bottom:
    .skip 1024 * 1024 * 4 #4MiB
.global stack_top
stack_top:
.global mbi
mbi:
    .skip 120

.section .rodata
/*gdt64:
    .quad 0 #null entry
#           executable      descriptor_type     present         64bit
.set code_segment, . - gdt64
    .quad   (1<<43)|        (1<<44)|            (1<<47)|        (1<<53) #code segment system
#                                                                           user
    .quad   (1<<43)|        (1<<44)|            (1<<47)|        (1<<53)|    (0b11<<45)#code segment user
    .quad                   (1<<44)|            (1<<47)|                    (0b11<<45) */
.global gdt64
gdt64:
	.quad 0		    #segmento nullo
code_sys_seg:
	.word 0b0           #limit[15:0]   not used
	.word 0b0           #base[15:0]    not used
	.byte 0b0           #base[23:16]   not used
	.byte 0b10011010    #P|DPL|1|1|C|R|A|  DPL=00=sistema
	.byte 0b00100000    #G|D|L|-|-------|  L=1 long mode
	.byte 0b0           #base[31:24]   not used
code_usr_seg:
	.word 0b0           #limit[15:0]   not used
	.word 0b0           #base[15:0]    not used
	.byte 0b0           #base[23:16]   not used
	.byte 0b11111010    #P|DPL|1|1|C|R|A|  DPL=11=utente
	.byte 0b00100000    #G|D|L|-|-------|  L=1 long mode
	.byte 0b0           #base[31:24]   not used
data_usr_seg:
	.word 0b0           #limit[15:0]   not used
	.word 0b0           #base[15:0]    not used
	.byte 0b0           #base[23:16]   not used
	.byte 0b11110010    #P|DPL|1|0|E|W|A|  DPL=11=utente
	.byte 0b00000000    #G|D|-|-|-------|
	.byte 0b0           #base[31:24]   not used
.global tss_seg
tss_seg:
	.word (tss_end-tss-1)           #limit[15:0]   
	.word 0           #base[15:0]    
	.byte 0           #base[23:16]   
	.byte 0x89    #P|DPL|1|0|E|W|A|  access
	.byte 0x40    #G|D|-|-|-------| flags
	.byte 0           #base[31:24]  
    .quad 0
gdt_end:

gdt64.pointer:
    .word gdt_end - gdt64 - 1
    .quad gdt64
.global tss
tss:
	.long 0
.global tss_stack_pointer0
.global tss_stack_pointer1
.global tss_stack_pointer2
tss_stack_pointer0:
	.quad 0
tss_stack_pointer1:
    .quad 0
tss_stack_pointer2:
    .quad 0
	.space 11 * 6, 0
	.word 0
	.word tss_end - tss - 1
.global tss_end
tss_end:
