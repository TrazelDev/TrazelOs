[bits 64]
DEFAULT REL

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


%macro isr_hardware_stub 1
isr_stub_%+%1:
  push qword 0
  push qword %1
  pushaq
  mov rdi, rsp
  call hardware_interrupts_isr_central_handler
  popaq
  add rsp, 16 ; undoing the pushes at the start
  iretq
%endmacro

[extern hardware_interrupts_isr_central_handler]
%assign i 32
%rep 224
  isr_hardware_stub i
%assign i i+1
%endrep

global isr_hardware_interrupt_stub_table
isr_hardware_interrupt_stub_table:
%assign i 32
%rep 224
  dq isr_stub_%+i
%assign i i+1
%endrep
