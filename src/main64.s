
.section .text
.code64
.global long_mode_start
long_mode_start:
    pushq $0
	popfq
    //load null into data segment registers
    mov $0, %ax
    mov %ax, %ss
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    //PRINT OK
    call kmain
    hlt
