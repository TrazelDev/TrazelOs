global asm_jump_usermode
asm_jump_usermode:
        mov rcx, rdi ; user mode function to jump
        mov rsp, rsi ; the user process stack
        mov r11, 0x202 ; the RFLAG value
        o64 sysret
