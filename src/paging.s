.section .text
.global set_current_trie
set_current_trie:
    push %rax
    mov %cr3, %rax
    cmp %rax, %rdi
    je .end
    mov %rdi, %cr3
.end:
    pop %rax 
    ret
