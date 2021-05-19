.section .text

.macro pushaq
    push %gs
    push %fs
    push %r15
    push %r14
    push %r13
    push %r12
    push %r11
    push %r10
    push %r9
    push %r8
    push %rbp
    push %rsp
    push %rdi
    push %rsi
    push %rdx
    push %rcx
    push %rbx
    push %rax
.endm

.macro popaq
    pop %rax
    pop %rbx
    pop %rcx
    pop %rdx
    pop %rsi
    pop %rdi
    pop %rsp
    pop %rbp
    pop %r8
    pop %r9
    pop %r10
    pop %r11
    pop %r12
    pop %r13
    pop %r14
    pop %r15
    pop %fs
    pop %gs
.endm

.global load_idt
//void load_idt(idtr_t& idtr)
load_idt:
    push %rbp
    mov %rsp, %rbp
    
	lidt (%rdi)

    pop %rbp
	ret

.global unkown_interrupt_wrapper
unkown_interrupt_wrapper:
    pushaq
    cld
    call unknown_interrupt
    popaq
    iretq

.global isr0
.global isr1
.global isr2
.global isr3
.global isr4
.global isr5
.global isr6
.global isr7
.global isr8
.global isr9
.global isr10
.global isr11
.global isr12
.global isr13
.global isr14
.global isr15
.global isr16
.global isr17
.global isr18
.global isr19
.global isr20
.global isr21
.global isr22
.global isr23
.global isr24
.global isr25
.global isr26
.global isr27
.global isr28
.global isr29
.global isr30
.global isr31
isr0:
    cli
    pushq $0
    pushq $0 
    jmp isr_common_handler_wrapper
isr1:
    cli
    pushq $0
    pushq $1 
    jmp isr_common_handler_wrapper
isr2:
    cli
    pushq $0
    pushq $2 
    jmp isr_common_handler_wrapper
isr3:
    cli
    pushq $0
    pushq $3 
    jmp isr_common_handler_wrapper
isr4:
    cli
    pushq $0
    pushq $4 
    jmp isr_common_handler_wrapper
isr5:
    cli
    pushq $0
    pushq $5 
    jmp isr_common_handler_wrapper
isr6:
    cli
    pushq $0
    pushq $6 
    jmp isr_common_handler_wrapper
isr7:
    cli
    pushq $0
    pushq $7 
    jmp isr_common_handler_wrapper
isr8:
    cli
    //pushq 1
    pushq $8 
    jmp isr_common_handler_wrapper
isr9:
    cli
    pushq $0
    pushq $9 
    jmp isr_common_handler_wrapper
isr10:
    cli
    //pushq 1
    pushq $10
    jmp isr_common_handler_wrapper
isr11:
    cli
    //pushq 1
    pushq $11
    jmp isr_common_handler_wrapper
isr12:
    cli
    //pushq 1
    pushq $12
    jmp isr_common_handler_wrapper
isr13:
    cli
    //pushq 1
    pushq $13
    jmp isr_common_handler_wrapper
isr14:
    cli
    //pushq 1
    pushq $14
    jmp isr_common_handler_wrapper
isr15:
    cli
    pushq $0
    pushq $15
    jmp isr_common_handler_wrapper
isr16:
    cli
    pushq $0
    pushq $16
    jmp isr_common_handler_wrapper
isr17:
    cli
    pushq $0
    pushq $17
    jmp isr_common_handler_wrapper
isr18:
    cli
    pushq $0
    pushq $18
    jmp isr_common_handler_wrapper
isr19:
    cli
    pushq $0
    pushq $19
    jmp isr_common_handler_wrapper
isr20:
    cli
    pushq $0
    pushq $20
    jmp isr_common_handler_wrapper
isr21:
    cli
    pushq $0
    pushq $21
    jmp isr_common_handler_wrapper
isr22:
    cli
    pushq $0
    pushq $22
    jmp isr_common_handler_wrapper
isr23:
    cli
    pushq $0
    pushq $23
    jmp isr_common_handler_wrapper
isr24:
    cli
    pushq $0
    pushq $24
    jmp isr_common_handler_wrapper
isr25:
    cli
    pushq $0
    pushq $25
    jmp isr_common_handler_wrapper
isr26:
    cli
    pushq $0
    pushq $26
    jmp isr_common_handler_wrapper
isr27:
    cli
    pushq $0
    pushq $27
    jmp isr_common_handler_wrapper
isr28:
    cli
    pushq $0
    pushq $28
    jmp isr_common_handler_wrapper
isr29:
    cli
    pushq $0
    pushq $29
    jmp isr_common_handler_wrapper
isr30:
    cli
    pushq $0
    pushq $30
    jmp isr_common_handler_wrapper
isr31:
    cli
    pushq $0
    pushq $31
    jmp isr_common_handler_wrapper

isr_common_handler_wrapper:
    pushaq
    //wip
    call isr_handler
    popaq
    add 16, %rsp
    iretq
