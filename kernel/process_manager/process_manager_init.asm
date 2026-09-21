global asm_jump_usermode
asm_jump_usermode:
        mov rcx, rdi ; user mode function to jump
        mov r11, 0x202 ; the RFLAG value
        mov rsp, 0x500000 ; the user process stack
        o64 sysret
