#pragma once

#define TURN_OFF_ALL_INTERRUPTS 0xFF
/// @brief function that is used to remap the programable interrupt controllers(pic).
/// the reason that this function exists is because when the IBM created the first home
/// computer, they designed it that by default that when a hardware interrupt is triggered
/// then the irq number is the index in the idt but the first 32 index are already 
/// taken by the software interrupts.
void remapPics();


#define IRQS_PER_PIC 8
#define LAST_INDEX_OF_MASTER_PIC IRQ::lpt1
#define LAST_INDEX_OF_SLAVE_PIC  IRQ::secondary_ata_hard_disk
enum class IRQ
{
    pit = 0,
    keyboard,
    cascade,   // this interrupt is used to connect the master and slave pics ( it is never raised in the idt )
    com2,
    com1,
    lpt2,
    floppy_disk,
    lpt1,
    cmos_rtc,
    free_for_peripherals1,
    free_for_peripherals2,
    free_for_peripherals3,
    ps2_mouse,
    fpu,
    primary_ata_hard_disk,
    secondary_ata_hard_disk,
};

// an updated bit mask of all of the new hardware interrupts
#define WORKING_HARDWARE_INTERRUPTS(currentWorkingHardwareInterrupts, indexOfNewHardwareInterrupt) (currentWorkingHardwareInterrupts & (~(1 << indexOfNewHardwareInterrupt)))
/// @brief function that enables the specific IRQ to generate interrupts if needed
void enableHardwareInterrupt(IRQ irqNumber);

// this is the value of EOI ( end of interrupt ) that is being signaled to the pic when the interrupt has been handled so interrupt can be used again;
#define END_OF_INTERRUPT 0x20 
/**
 * @brief the function is getting an irq number and signals that the hardware
 *  interrupt has ended and another hardware interrupt can now be executed
*/
void picSendEOI(IRQ irqNumber);