
section .data
    hello db 'Hello, Bro', 0Ah

section .text
    global _start

_start:
    mov eax, 4
    mov ebx, 1
    lea ecx, [hello]
    mov edx, 13
    int 0x80

    ; Exit the program
    mov rax, 60
    mov rdi, 0
    syscall