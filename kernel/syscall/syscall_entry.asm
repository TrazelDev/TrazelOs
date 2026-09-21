[extern syscall_kernel_handler]
global asm_kernel_syscall_entrypoint

%macro popaq 0
  pop rax
  pop rbx
  pop rcx
  pop rdx
  pop rsi
  pop rdi
  pop rbp
  pop r8
  pop r9
  pop r10
  pop r11
  pop r12
  pop r13
  pop r14
  pop r15
%endmacro

%macro pushaq 0
  push r15
  push r14
  push r13
  push r12
  push r11
  push r10
  push r9
  push r8
  push rbp
  push rdi
  push rsi
  push rdx
  push rcx
  push rbx
  push rax
%endmacro

asm_kernel_syscall_entrypoint:
        swapgs
        mov gs:[0x8], rsp ; saving ring3 rsp
        mov rsp, gs:[0]   ; extracting ring0 rsp

        push 0 ; data segment that came from is irelavant
        push qword gs:[0x8]
        push r11 ; rflags at syscall (syscall saves rflags register into r11 cause sfmask changes it)
        push 0 ; code segment that came from is irelavant
        push rcx ; rip to return (syscall saves it into rcx)
        push 0 ; potential error code (there are no error codes in syscalls)
        push -1 ; this is the interrupt index (but this is sycall not interrupt so no index needed)
        pushaq

        mov rdi, rsp ; pointer to the struct interrupt_info as the first function param
        call syscall_kernel_handler


        popaq
        add rsp, 16 ; skipping interrupt index and error code
        pop rcx ; restoring rip
        add rsp, 8 ; skiping code segment
        pop r11 ; restore r11

        mov rsp, gs:[0x8] ; restoring ring3 rsp
        swapgs
        o64 sysret
