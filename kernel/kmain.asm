; global jump_usermode
; jump_usermode:
; ;enable system call extensions that enables sysret and syscall
; 	mov rcx, 0xc0000082
; 	wrmsr
; 	mov rcx, 0xc0000080
; 	rdmsr
; 	or eax, 1
; 	wrmsr
; 	mov rcx, 0xc0000081
; 	rdmsr
; 	mov edx, 0x00180008
; 	wrmsr

; 	mov ecx, 0x400000 ; to be loaded into RIP
; 	mov r11, 0x202 ; to be loaded into EFLAGS
; 	o64 sysret
;

global jump_usermode
jump_usermode:
    ; Enable system call extensions
    mov rcx, 0xc0000080
    rdmsr
    or eax, 1
    wrmsr

    ; Configure STAR MSR for hardware segment calculation
    mov rcx, 0xc0000081
    rdmsr
    mov edx, 0x00100008  ; <--- MUST BE 0x10, NOT 0x18
    wrmsr

    ; Prepare hardware registers for the privilege drop
    mov ecx, 0x400000    ; to be loaded into RIP
    mov r11, 0x202       ; to be loaded into EFLAGS
    
    ; SWAP THE STACK: sysret does not do this automatically!
    mov rsp, 0x401000    ; <--- MUST MANUALLY SET USER STACK

    o64 sysret
