
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

    push %rdi
///call global initializers
	movabs $__init_array_start, %rbx
1:	cmpq $__init_array_end, %rbx
	je 2f
	call *(%rbx)
	addq $8, %rbx
	jmp 1b
	// il resto dell'inizializzazione e' scritto in C++
2:	

    call init_tss

    popq %rdi
    call kmain
    hlt

init_tss:
    movq $tss_seg, %rdx
	movw $(104 - 1), (%rdx) 	#[15:0] = limit[15:0]
	movq $tss, %rax
	movw %ax, 2(%rdx)	#[31:16] = base[15:0]
	shrq $16,%rax
	movb %al, 4(%rdx)	#[39:32] = base[24:16]
	movb $0b10001001, 5(%rdx)	#[47:40] = p_dpl_type
	movb $0, 6(%rdx)	#[55:48] = 0
	movb %ah, 7(%rdx)	#[63:56] = base[31:24]
	shrq $16, %rax
	movl %eax, 8(%rdx) 	#[95:64] = base[63:32]
	movl $0, 12(%rdx)	#[127:96] = 0

	movw $(32), %cx
	ltr %cx

    retq
