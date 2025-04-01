readDisk:
	call findBootPartion

	mov ah, 0x02 ; Bios disk read number
	mov bx, [bootloader_addr] ; Bootloader load addr
	mov al, [bootloader_size] ; Sectors to load
	mov dl, [boot_disk]       ; Drive number
	mov ch, 0x00              ; Cylinder number
	mov dh, 0x00              ; Head number
	mov cl, 0x02              ; Sector number
	int 0x13                  ; Bios disk interrupt


	jc diskReadFailed
	ret

diskReadFailed:
	mov si, disk_fail_msg
	call printString
	jmp $


findBootPartion:
	mov bx, [partion_table_addr]
	mov cx, 4 ; Max partions at mbr

	try_find_boot_partion:
		mov al, [bx] ; partion entry first byte
		test al, 0b10000000 ; boot flat bit
		jz not_boot_partion
		
		; loading partion details
		; ...
		ret 

		not_boot_partion:
			add bx, 0x10
			loop try_find_boot_partion
			jmp noBootPartion

noBootPartion:
	mov si, no_boot_partion_msg
	call printString
	jmp $

bootloader_addr: dw 0
bootloader_size: db 0
boot_disk: db 0
partion_table_addr: dw 0x7dbe
disk_fail_msg: db 'Disk Read Failed!', 0x0
no_boot_partion_msg: db 'No bootable partion!', 0x0
