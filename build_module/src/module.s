.code64
.section .text

.global _start
_start:
    push %rdi
    push %rsi

///call global initializers
	movabs $__preinit_array_start, %rbx
1:	movabs $__preinit_array_end, %rcx
    cmpq %rcx, %rbx
	je 2f
	call *(%rbx)
	addq $8, %rbx
	jmp 1b
2:
    ///call global initializers
	movabs $__init_array_start, %rbx
1:	movabs $__init_array_end, %rcx
    cmpq %rcx, %rbx
	je 2f
	call *(%rbx)
	addq $8, %rbx
	jmp 1b
2:

    pop %rsi
    pop %rdi
    call main
    push %rax

///call global initializers
	movabs $__fini_array_start, %rbx
1:	movabs $__fini_array_end, %rcx
    cmpq %rcx, %rbx
	je 2f
	call *(%rbx)
	addq $8, %rbx
	jmp 1b
2:

    pop %rdi
    call fin
