; this is the null descriptor. it is not used by the processor but needs to be present as the first descriptor in the gdt:
gdt_nulldesc:
	dd 0
	dd 0

gdt_codedesc:
	dw 0xFFFF	; limit field. it specifies the size of the segment. here, it is set to 0xFFFF, which means the size of the segment is 64KB.		
    ; these are the base fields. they specify the starting address of the segment. here, they are set to 0, which means the segment starts at address 0.
	dw 0x0000   
	db 0x00		
    
    ; bit 7 (1): this is the present bit. it indicates whether the segment is present in memory or not. if it is set to 1, the segment is present.
    ; bit 6 and 5 (00): these are the privilege bits. they specify the privilege level of the segment. 00 means the segment can be accessed by any privilege level.
    ; bit 4 (1): this is the descriptor type bit. if it is set to 1, the descriptor is for a code or data segment.
    ; bit 3 (1): this is the executable bit. if it is set to 1, the segment is a code segment. if it is set to 0, the segment is a data segment.
    ; bit 2 (0): this is the direction bit for data segments or the conforming bit for code segments. for code segments, if it is set to 1, the code segment can be executed from an equal or lower privilege level. for data segments, if it is set to 1, the segment grows down.
    ; bit 1 (1): this is the readable bit for code segments or the writable bit for data segments. for code segments, if it is set to 1, the segment can be read.
    ; bit 0 (0): this is the accessed bit. it is set to 1 by the processor when the segment is accessed.
	db 10011010b		

    ; bit 7 (1): this is the granularity bit. if it is set to 1, the limit is multiplied by 4KB to calculate the size of the segment.
    ; bit 6 (1): this is the size bit. if it is set to 1, the segment is 32-bit. if it is set to 0, the segment is 16-bit.
    ; bit 5 (0): this is the available bit. it is not used by the processor and can be used for operating system-specific purposes.
    ; bit 4 (0): this is the long mode bit. it is used in 64-bit mode. here, it is set to 0, which means the segment is not a 64-bit segment.
    ; bit 3 (1): this is the default operation size bit. if it is set to 1, the segment uses 32-bit operands. if it is set to 0, the segment uses 16-bit operands.
    ; bit 2, 1, 0 (111): these are the limit bits. they are the upper three bits of the limit field. here, they are set to 111, which means the limit is 0xF.
	db 11001111b	
	db 0x00		

gdt_datadesc:
	dw 0xFFFF
	dw 0x0000
	db 0x00
	db 10010010b
	db 11001111b
	db 0x00

gdt_end: ; the address of the end of the gdt 

gdt_descriptor:
	gdt_size: 
		dw gdt_end - gdt_nulldesc - 1
		dq gdt_nulldesc


codeseg equ gdt_codedesc - gdt_nulldesc
dataseg equ gdt_datadesc - gdt_nulldesc

[bits 32]

editGDT:
	; changing the access bits so they will fit to 64 bit system 
	; Bit 7 (1): Segment is present in memory.
	; Bit 6 (0): The descriptor privilege level (DPL) is 0, meaning the segment can be accessed by any privilege level.
	; Bit 5 (1): This is a descriptor for a code or data segment (not a system segment).
	; Bit 4 (0): The segment is a data segment (since the executable bit is 0).
	; Bit 3 (1): The segment grows up (since the direction bit is 1 for data segments).
	; Bit 2 (1): The segment is writable (since the writable bit is 1 for data segments).
	; Bit 1 (1): The segment has been accessed (since the accessed bit is 1).
	; Bit 0 (1): The granularity bit is set to 1, meaning the limit is in 4KB blocks, not bytes.
	mov [gdt_codedesc + 6], byte 10101111b
	mov [gdt_datadesc + 6], byte 10101111b
	ret

[bits 16]