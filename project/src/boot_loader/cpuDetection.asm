
; this function is checking if the cpu is supporgin the cpuid instruction 
checkCpuId:
    ; this is saving the flag register in eax by pushing it to the stack and poping it to eax
    pushfd 
	pop eax

    mov ecx, eax     ; saves the original flag register for later
    xor eax, 1 << 21 ; this is fliping the 21 bit
    


    ; it looks like the eax should not change at all but if it is chaning it means
    ; that the bit was not allowed to be flipped and the cpu does not support the cpuid instruction:

    ; puts the flags of eax into the flag register 
    push eax
	popfd
    ; this is instrction are saving the the flags back in eax
	pushfd
	pop eax


    ; this resotre the orignal flags of the flag register
    push ecx 
	popfd

    ; checking if the bit was allowed to be filpped or not like was explained earlier:
    xor eax,ecx
	jz cpuDoesNotSupportCPUIDinstruction
	ret

detectLongMode:
    mov eax, 0x80000001 ; this value is an input to the cpuid instruction to request extended processor info
    ; this is an instruction in protected mode which provides details about the the processor
    ; the instruction is outputing values in eax, ebx, ecx and edx depending on the input 
    cpuid               
    
    ; checking if the 29 bit is turned on or not
    ; this bit indicates if the cpu supports long mode or not
    test edx, 1 << 29   
    jz noLongMode
    ret

noLongMode:
    hlt

cpuDoesNotSupportCPUIDinstruction:
    hlt
