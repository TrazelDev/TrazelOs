PROGRAM_SPACE equ 0x9000   	 ; programm space is both bootloade code and kernel code



readDisk:
	mov ah, 0x02             ; bios function number for disk read
	mov bx, PROGRAM_SPACE    ; address of where the data is loaded
	mov al, SECTORS_TO_LOAD  ; the amount of sectors to read each sector is 512 bytes, this variable is specified in the assembly of the mbr
	mov dl, [BOOT_DISK]      ; the drive number
	mov ch, 0x00
	mov dh, 0x00             
	mov cl, 0x02             ; the number of the first sector

	int 0x13                 ; the bios disk service in interrupt

	jc diskReadFailed
	ret

BOOT_DISK: db 0

diskReadErrorString: db 'Disk Read Failed!', 0

diskReadFailed:
	mov si, diskReadErrorString
	call printString
	jmp $ ; infinite jump in the case that we fail to load from the disk