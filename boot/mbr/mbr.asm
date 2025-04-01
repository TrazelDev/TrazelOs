[org 0x7c00] ; the origin that the bios loads the mbr in


; in the start of the programm we are the driver number is in dl so we save it for later use
mov [boot_disk], dl

mov bp, [stack_ptr_addr]
mov sp, bp

; Load bootloader:
call readDisk
jmp bootloader_addr

%include "print.asm"
%include "diskRead.asm"
stack_ptr_addr: dw 0x8000


; Padding mbr to be a full sector:
times 510-($-$$) db 0

dw 0xaa55
