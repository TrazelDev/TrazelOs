jmp EnterProtectedMode

errorString: db 'failed', 0
%include "gdt.asm"
%include "memoryMap.asm"

EnterProtectedMode:
	call detectMemory
	call enableA20
	cli						   ; clearing the interrupt flag
	lgdt [gdt_descriptor]	   ; this loads the gdt into the gdt registers
	mov eax, cr0			   ; this moves the control register (cr0) which controls aspects in the os
	or eax, 1				   ; setting the least sagnificit bit which says that switching to protected mode 
	mov cr0, eax

	; jumping into protected mode code:
	jmp codeseg:startProtectedMode	; preforming far jump which is necessary because the cpu needs to switch to a new code segment after switching to 32 bit

; this gives the option to access more then 1 mb in real mode this is enabled to be comptiable with older version
enableA20:
	in al, 0x92  ; read from port 0x92
	or al, 2	 ; set bit 1 in the value read
	out 0x92, al ; write the modified value back to port 0x92
	ret

[bits 32]
%include "cpuDetection.asm"
%include "identityPaging.asm"

startProtectedMode:
	mov ax, dataseg
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	call checkCpuId			  ; making sure that the cpu that we are working with supports the cpuid instrecution
	call detectLongMode		  ; make sure that the cpu supports long mode
	call setUpIdentityPaging  ; setting up identity paging for the first 512 pages
	call editGDT			  ; chaning the gdt so it will now be in protected mode

	
	; switching to long mode:
	jmp codeseg:start64Bit ; preforming far jump which is necessary because the cpu needs to switch to a new code segment after switching to 64 bit

[bits 64]
[extern bootloader_entry]
start64Bit:
	; cleaning the text and changing the text color to green
	mov edi, 0xb8000
	mov rax, 0x0a200a200a200a20
	mov ecx, 500
	rep stosq
	
	jmp bootloader_entry

	pauseCpu:
		hlt
		jmp pauseCpu

times 0x400-($-$$) db 0

