[org 0x7c00] ; the origin that the bios loads the mbr in


; in the start of the programm we are the driver number is in dl so we save it for later use
mov [BOOT_DISK], dl 

mov bp, 0x7c00
mov sp, bp

; reading some sectors from the disk in order for us to load the bootloader
call readDisk
jmp PROGRAM_SPACE

; including some src files:
%include "src/mbr/print.asm"
%include "src/mbr/diskRead.asm"

errorString: db 'failed', 0 

; adding zeros in the end of the sector so there will the signiture 0xaa55 at the end of the file:
times 510-($-$$) db 0

dw 0xaa55