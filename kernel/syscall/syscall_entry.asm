global asm_kernel_syscall_entrypoint
global asm_jump_usermode
asm_jump_usermode:
        mov rcx, rdi ; user mode function to jump
        mov r11, 0x202 ; the RFLAG value
        mov rsp, 0x401000 ; the user process stack
        o64 sysret

asm_kernel_syscall_entrypoint:
        cli
        sub rcx, 2
        o64 sysret
        hlt

