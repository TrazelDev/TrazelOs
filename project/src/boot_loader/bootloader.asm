jmp EnterProtectedMode

errorString: db 'failed', 0 
%include "src/boot_loader/gdt.asm"
%include "src/mbr/print.asm"

EnterProtectedMode:
	call enableA20    
	cli 					   ; clearing the interrupt flag
	lgdt [gdt_descriptor]      ; this loads the gdt into the gdt registers
	mov eax, cr0			   ; this moves the control register (cr0) which controls aspects in the os
	or eax, 1				   ; setting the least sagnificit bit which says that switching to protected mode 
	mov cr0, eax	

    ; jumping into protected mode code:
    jmp codeseg:startProtectedMode  ; preforming far jump which is necessary because the cpu needs to switch to a new code segment after switching to 32 bit

; this gives the option to access more then 1 mb in real mode this is enabled to be comptiable with older version
enableA20:
	in al, 0x92  ; read from port 0x92
	or al, 2     ; set bit 1 in the value read
	out 0x92, al ; write the modified value back to port 0x92
	ret

[bits 32]
; %include "bootloader_stage/cpuDetection.asm"
; %include "bootloader_stage/identityPaging.asm"

startProtectedMode:
    mov ax, dataseg
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

    ; printing hello world:
    mov [0xb8000], byte 'H'
	mov [0xb8002], byte 'e'
	mov [0xb8004], byte 'l'
	mov [0xb8006], byte 'l'
	mov [0xb8008], byte 'o'
	mov [0xb800a], byte ' '
	mov [0xb800c], byte 'W'
	mov [0xb800e], byte 'o'
	mov [0xb8010], byte 'r'
	mov [0xb8012], byte 'l'
	mov [0xb8014], byte 'd'
	mov [0xb8016], byte '!'
	mov [0xb8018], byte ' '
    mov [0xb801A], byte ' '
    mov [0xb801C], byte ' '
    mov [0xb801E], byte ' '
    mov [0xb8020], byte ' '
    mov [0xb8022], byte ' '
    mov [0xb8024], byte ' '
    mov [0xb8026], byte ' '
    mov [0xb8028], byte ' '
    mov [0xb802A], byte ' '

    
    jmp $
times 32650-($-$$) db 0