global _start
_start:
        mov rax, str
        syscall
        syscall
        syscall
        jmp $

str: db 'Hello world from user space', 10, 0x0
