; function that is used to print things while we are in the master boot record stage:
printString:
    ; loading the next char of the string into al
    lodsb

    ; checking if al is 0 ( null ) and this is the end of the string
    or al, al
    jz printStringDone

    ; printing the char using the bios interrupt
    mov ah, 0x0E
    int 0x10

    jmp printString

printStringDone:
    ret