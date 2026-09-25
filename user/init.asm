global _start

_start:
        mov rax, 1
        mov rdi, 1
        mov rsi, str
        mov rdx, 27
        syscall
        jmp $

str: db 'Hello world from user space', 10, 0x0
