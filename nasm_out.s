section .data
x dq 0

section .text
global _start

_start:
    mov rax, 2
    mov rbx, rax
    mov rax, 3
    add rax, rbx
    mov [x], rax
    mov rax, [x]
    mov rdi, rax
    mov rax, 60
    syscall
