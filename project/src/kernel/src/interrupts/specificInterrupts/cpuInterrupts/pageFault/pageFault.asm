; this is an auto generated file
[bits 64]
REGISTER_COUNT          equ 15
REGISTER_SIZE_IN_BYTES  equ 8
%include "src/kernel/src/interrupts/specificInterrupts/popAndPushAll.inc"

[extern isrPageFaultHandler]
asmIsrPageFaultHandler:
  pushaq

  mov rdi, [rsp+REGISTER_COUNT*REGISTER_SIZE_IN_BYTES] ; passing the error code into the isr function in cpp
  call isrPageFaultHandler

  popaq

  add rsp, 8 ; adjust the stack pointer to release the space allocated for the error code
  sti
  iretq ; returning from the interrupt
  global asmIsrPageFaultHandler
