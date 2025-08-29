readDisk:
	call findBootPartion
	call getBootloaderSize

	mov ah, 0x02                        ; Bios disk read number
	mov bx, [bootloader_addr]           ; Bootloader load addr
	mov al, [bootloader_size]           ; Sectors to load
	mov dl, [boot_disk]                 ; Drive number
	mov ch, [bootloader_start_cylinder] ; Cylinder num
	mov dh, [bootloader_start_head]     ; Head num
	mov cl, [bootloader_start_sector]   ; Sector num
	int 0x13                            ; Bios disk interrupt


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

		; The bootloader only supports fat12 for now
		mov al, [bx + 4]
		cmp al, [fat12_system_id]
		jne not_fat12_id

		; loading partion details
		mov al, [bx + 1]
		mov [bootloader_start_head], al
		mov al, [bx + 2]
		mov [bootloader_start_sector], al
		mov al, [bx + 3]
		mov [bootloader_start_cylinder], al
		ret
	
		not_fat12_id: ; The bootloader only supports fat12 for now
		not_boot_partion:
			add bx, 0x10
			loop try_find_boot_partion
			jmp noBootPartion


getBootloaderSize:
	; Reading the first sector of the bootloader:
	mov ah, 0x02                        ; Bios disk read number
	mov bx, [bootloader_addr]           ; Bootloader load addr
	mov al, 1                           ; Sectors to load
	mov dl, [boot_disk]                 ; Drive number
	mov ch, [bootloader_start_cylinder] ; Cylinder num
	mov dh, [bootloader_start_head]     ; Head num
	mov cl, [bootloader_start_sector]   ; Sector num
	int 0x13                            ; Bios disk interrupt
	jc diskReadFailed
	
	; Getting the resevered sector count from the FAT12 BPB
	; ( The rerserved sector count contains the actual bootloader binnary )
	mov bx, [bootloader_addr]
	mov al, [bx + 0xe]
	mov [bootloader_size], al
	ret


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
no_boot_partion_msg: db 'No fat12 bootable partion!', 0x0
fat12_system_id: db 1
