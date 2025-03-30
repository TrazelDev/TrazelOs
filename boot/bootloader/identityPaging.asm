PageTableEntry equ 0x1000

; here we are setting indenity paging which is the basis for setting paging in the system ( this is crutial for switching to long mode )
; we are setting this paging by seetting the cr3 register which holds the pointer to the plm4 table and just putting there 1 entry to point to our 
; pdpt, and in the pdpt we make one entry to point to pd and in there 1 entry to point to the pt.
; in the pt we are putting 512 entries to point to the first 512 pages in the system, that means that we cannot use more then 512 * 4k address out of the box
setUpIdentityPaging:
	; setting the cr3 register to the plm4 table entry ( cr3 register cannot be set with an immediate value so we set it using another register )
	mov edi, PageTableEntry   ; also setting the edi to the second page ( index 1 ) to set values for memory
	mov cr3, edi			  
    
    ; here we are setting the second page in the system to be for pml4 ( page map level 4 )
	mov dword [edi], 0x2003 ; setting the the first entry of the plm4 to have the index 2 ( which is the third page and the place we have the pdpt table)
    add edi, 0x1000         ; increasing the value by 0x1000 ( 4k ) to get to the next page


    mov dword [edi], 0x3003 ; setting the first entry in the pdpt to point to the fourth page where pd is in
    add edi, 0x1000         ; increasing the value by 0x1000 ( 4k ) to get to the next page    


    mov dword [edi], 0x4003 ; setting the first entry in the pd to point to the pt
    add edi, 0x1000         ; increasing the value by 0x1000 ( 4k ) to get to the next page         

    ; now we are a going in a loop and setting all 512 page tables entries of this page table to point to the first 512 pages in the system: 
    mov ebx, 0x00000003         
    mov ecx, 512                
	.SetEntry:
		mov dword [edi], ebx    
		add ebx, 0x1000         
		add edi, 8              
		loop .SetEntry          

	; Enable 4-MB pages by setting the CR4.PSE bit
    mov eax, cr4
    or eax, 1 << 5             ; Set the CR4.PSE bit ( page size extention )
    mov cr4, eax

	; Enable global pages by modifying the MSR
    ; creating automatic tlb chashing for pages that are frequently used
    mov ecx, 0xC0000080         ; address of the MSR_IA32_MCG_CAP register
    rdmsr                       ; Read the MSR
    or eax, 1 << 8              ; set the global pages (G) bit
    wrmsr                       ; write the MSR

    ; Enable paging by setting the CR0.PG bit
    ; cr0 is a register that contains bits that allowe certain opertaions
    mov eax, cr0
    or eax, 1 << 31            ; Set the CR0.PG bit
    mov cr0, eax

	ret



; __  __                            _        _          _  _                               
;|  \/  |  ___   _ __  ___       __| |  ___ | |_  __ _ (_)| | ___                          
;| |\/| | / _ \ | '__|/ _ \     / _` | / _ \| __|/ _` || || |/ __|                         
;| |  | || (_) || |  |  __/    | (_| ||  __/| |_| (_| || || |\__ \                         
;|_|  |_|_\___/ |_|   \___|   _ \__,_| \___|_\__|\__,_||_||_||___/_                        
;  __ _ | |__    ___   _   _ | |_     | |_ | |__    ___      / _|| |  __ _   __ _  ___   _ 
; / _` || '_ \  / _ \ | | | || __|    | __|| '_ \  / _ \    | |_ | | / _` | / _` |/ __| (_)
;| (_| || |_) || (_) || |_| || |_     | |_ | | | ||  __/    |  _|| || (_| || (_| |\__ \  _ 
; \__,_||_.__/  \___/  \__,_| \__|     \__||_| |_| \___|    |_|  |_| \__,_| \__, ||___/ (_)
;                                                                           |___/          
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Present (P): Bit 0
; Indicates whether the page is present in physical memory.
; If set (1), the page is present.
; If clear (0), the page is not present, and a page fault will occur if an attempt is made to access the page.

; Read/Write (R/W): Bit 1
; Controls whether the page is writable or read-only.
; If set (1), the page is read/write.
; If clear (0), the page is read-only.

; User/Supervisor (U/S): Bit 2
; Determines the privilege level required for accessing the page.
; If set (1), the page can be accessed by user-level (ring 3) and supervisor-level (ring 0) code.
; If clear (0), the page is accessible only by supervisor-level code.

; Write-Through (W/T): Bit 3
; Controls the caching policy for the page.
; If set (1), write-through caching is enabled.
; If clear (0), write-back caching is enabled.

; Cache Disable (C/D): Bit 4
; Determines whether caching is enabled for the page.
; If set (1), caching is disabled.
; If clear (0), caching is enabled.

; Accessed (A): Bit 5
; Set by the processor when the page is accessed (read or write).
; Can be used by the operating system for page replacement algorithms.

; Dirty (D): Bit 6
; Set by the processor when a write operation is performed on the page.
; Can be used by the operating system for page replacement and swapping.

; Page Size (PS): Bit 7 (for PSE, Page Size Extension)
; In the context of larger pages, such as 4 MB pages, this flag indicates the page size.
; If set (1), a larger page size is used (e.g., 4 MB).

; Global (G): Bit 8
; Used for controlling the behavior of the TLB (Translation Lookaside Buffer).
; If set (1), the TLB entry is marked as global, and it is not invalidated when CR3 is loaded with a new page directory.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; website to convert words to words with ascii art: https://www.asciiart.eu/text-to-ascii-art