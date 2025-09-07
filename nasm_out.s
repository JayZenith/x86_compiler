global _start
_start:
    mov rax, 2
    push rax
    mov rax, 3
    pop rbx
    add rax, rbx
    push rax
    mov rax, [rsp + 0]
    mov rdi, rax
    mov rax, 60
    syscall
    mov rax, 60
    mov rdi, 0
    syscall
