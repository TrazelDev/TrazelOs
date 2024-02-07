#include "pic.h"
#include "src/interrupts/idtTypes/idtIndexes.h"

#include "utility/utility.h"


// ICW - Initialization Command Word
#define ICW_INIT 0x10 // tells the pic that it will expect initialization commands rather then standard ones
#define ICW_ICW4 0x01 // tells the pic that the initialization will include the icw4 which tells the mode that will used for pic 
#define ICW_8086 0x01


void remapPics()
{
    // saving the current data of the master and salve pics to restore later: 
    uint8_t masterPicPreviousData = inb(IO_PORTS::master_pic_data_port);   
    uint8_t slavePicPreviousData  = inb(IO_PORTS::slave_pic_data_port );

    // initializing the pic with specific flags:
    outb(IO_PORTS::master_pic_command_port, ICW_INIT | ICW_ICW4);
    outb(IO_PORTS::slave_pic_command_port , ICW_INIT | ICW_ICW4);

    // setting the interrupt descriptor offset for each PIC
    outb(IO_PORTS::master_pic_data_port, InterruptIndexes::pit     ); // setting the first idt offset for the master pic
    outb(IO_PORTS::slave_pic_data_port , InterruptIndexes::cmos_rtc); // setting the first idt offset for the slave pic

    // set x86 mode of pics for both pics for interrupt driven os:
    outb(IO_PORTS::master_pic_data_port, ICW_8086);
    outb(IO_PORTS::slave_pic_data_port , ICW_8086);

    // restoring the values of the master and slave pics:
    outb(IO_PORTS::master_pic_data_port, masterPicPreviousData);
    outb(IO_PORTS::slave_pic_data_port , slavePicPreviousData );
}

void enableInterrupt(IRQ irqNumber)
{
    // this is a bit mask that represents the current irq numbers that generate interrupts, 
    // 1 - doesn't generate interrupts. 0 - generates interrupts
    uint8_t workingInterruptsBitMask = 0;
    uint8_t portOfTheIRQ = (irqNumber > LAST_INDEX_OF_MASTER_PIC); // a value that stores the port ( slave port or master port ) that corresponds to this specific IRQ number

    if(irqNumber > LAST_INDEX_OF_MASTER_PIC) { portOfTheIRQ = IO_PORTS::slave_pic_data_port;  }
    else                                     { portOfTheIRQ = IO_PORTS::master_pic_data_port; }

    workingInterruptsBitMask = inb(portOfTheIRQ);
    outb(portOfTheIRQ, workingInterruptsBitMask | (irqNumber % IRQS_PER_PIC));    
}

void picSendEOI(IRQ irqNumber)
{
    // in the case that the handled interrupt was on the slave pic:
    if(irqNumber > LAST_INDEX_OF_MASTER_PIC)
    {
        outb(IO_PORTS::slave_pic_data_port, END_OF_INTERRUPT);
        return;
    }
    

    // in the case that the handled interrupt was on the master pic:
    outb(IO_PORTS::master_pic_data_port, END_OF_INTERRUPT);
}