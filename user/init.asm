global _start

_start:
        mov rax, str
        mov rcx, 3
start_loop:
        push rcx
        syscall
        pop rcx
        loop start_loop
        jmp $

str: db 'Hello world from user space', 10, 0x0
