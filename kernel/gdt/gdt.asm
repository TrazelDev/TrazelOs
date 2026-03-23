[bits 64]
DEFAULT REL


global gdt_reload_segments
gdt_reload_segments:
	; rdi - code selector
	; rsi - data selector
	mov ax, si
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	push rdi
	lea rax, [rel .reload_cs]
	push rax
	retfq

.reload_cs:
	ret
