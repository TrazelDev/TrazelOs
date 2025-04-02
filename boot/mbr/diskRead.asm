readDisk:
	call findBootPartion

	mov ah, 0x02 ; Bios disk read number
	mov bx, [bootloader_addr] ; Bootloader load addr
	mov al, [bootloader_size] ; Sectors to load
	mov dl, [boot_disk]       ; Drive number
	mov ch, [bootloader_start_cylinder] ; Cylinder num
	mov dh, [bootloader_start_head]     ; Head num
	mov cl, [bootloader_start_sector]   ; Sector num
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
		mov al, [bx + 1]
		mov [bootloader_start_head], al
		mov al, [bx + 2]
		mov [bootloader_start_sector], al
		mov al, [bx + 3]
		mov [bootloader_start_cylinder], al
		mov al, [bx + 0x0c]
		mov [bootloader_size], al
		ret 

		not_boot_partion:
			add bx, 0x10
			loop try_find_boot_partion
			jmp noBootPartion

noBootPartion:
	mov si, no_boot_partion_msg
	call printString
	jmp $

bootloader_addr: dw 0x8000
boot_disk: db 0


bootloader_start_cylinder: db 0
bootloader_start_head: db 0
bootloader_start_sector: db 0
bootloader_size: db 0

partion_table_addr: dw 0x7dbe
disk_fail_msg: db 'Disk Read Failed!', 0x0
no_boot_partion_msg: db 'No bootable partion!', 0x0
test: db 'Hello world', 0x0
