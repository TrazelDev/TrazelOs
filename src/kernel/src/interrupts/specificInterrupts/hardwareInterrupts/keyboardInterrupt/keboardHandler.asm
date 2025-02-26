[bits 64]

%include "src/kernel/src/interrupts/specificInterrupts/popAndPushAll.inc"

[extern isrKeyboardHandler]
asmIsrKeyboardHandler:
  pushaq
  call isrKeyboardHandler
  popaq

  sti
  iretq ; returning from the interrupt
  global asmIsrKeyboardHandler