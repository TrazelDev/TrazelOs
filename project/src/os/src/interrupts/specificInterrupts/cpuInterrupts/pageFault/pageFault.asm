; this is an auto generated file
[bits 64]
%include "src/os/src/interrupts/specificInterrupts/popAndPushAll.inc"

[extern isrPageFaultHandler]
asmIsrPageFaultHandler:
  pushaq
  call isrPageFaultHandler
  popaq

  sti
  iretq ; returning from the interrupt
  global asmIsrPageFaultHandler
