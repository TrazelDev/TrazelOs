; defines to make everything more clear:
SMAP_ASCII_VALUE                   equ 0x534D4150
BIOS_FUNCTION_TO_QUERY_MEMORY_MAPS equ 0xE820
BIOS_SYSTEM_SERVICE_INTERRUPT      equ 0x15
MEMORY_MAP_SIZE                    equ 24

; global variables that are used in cpp kernel code:
addressWithMemoryMaps:
	dq 0x5002
	global addressWithMemoryMaps
memoryRegionCount:
    db 0
    global memoryRegionCount

detectMemory:
	mov ax, 0
	mov es, ax                       ; set the ax to the value 0 ( we are using ax because we cannot move a direct value into es)
	mov di, [addressWithMemoryMaps]  ; place that we would like to store it ( it is random )
	mov edx, SMAP_ASCII_VALUE        
	xor ebx, ebx

	.repeat:
		mov eax, BIOS_FUNCTION_TO_QUERY_MEMORY_MAPS 
		mov ecx, MEMORY_MAP_SIZE
		int BIOS_SYSTEM_SERVICE_INTERRUPT

		cmp ebx, 0                   ; check if the carry flag is set (indicates end of memory map)
		je .finished    

		add di, MEMORY_MAP_SIZE      ; move to the next memory region entry
		inc byte [memoryRegionCount] ; increment the memory region count
		jmp .repeat

	.finished:
	ret