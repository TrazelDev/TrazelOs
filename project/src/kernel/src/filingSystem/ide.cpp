#include "ide.h"
#include "ideDefinitions.h"

static IDEChannelRegisters driveChanel[DRIVE_CHANEL_NUMBER::CHANEL_COUNT];
#define GET_CHANEL_PORT(driveIndex, portType) (driveChanel[driveIndex > 1].portType)
#define CONVERT_DRIVE_INDEX_TO_CHANEL_INDEX(driveIndex) (DRIVE_CHANEL_NUMBER)(driveIndex >= DRIVE_CHANEL_NUMBER::CHANEL_COUNT)
#define GET_DRIVE_TYPE(driveIndex)            ((driveIndex % DRIVE_CHANEL_NUMBER::CHANEL_COUNT) ? MASER_DRIVE : SLAVE_DRIVE)

void ideInit(DRIVE_NUMBER driveIndex)
{
    DRIVE_CHANEL_NUMBER chanelIndex = CONVERT_DRIVE_INDEX_TO_CHANEL_INDEX(driveIndex);
    ASSERT_PRINT_ERROR(chanelIndex == DRIVE_CHANEL_NUMBER::MASTER_CHANEL, printf("Error: Currently there is no support for the slave drives"));


    if(chanelIndex == DRIVE_CHANEL_NUMBER::MASTER_CHANEL)
    {
        driveChanel[chanelIndex].basePort    = MASTER_IDE_IO_PORT;
        driveChanel[chanelIndex].controlPort = MASTER_IDE_CONTROL_PORT;
        driveChanel[chanelIndex].busMasterInterfacePort = 0; // currently does not support that
        driveChanel[chanelIndex].noInterrupt = false;
    }

    // I do not know what is it doing yet needs to be checked:
    outb(driveChanel[chanelIndex].basePort + features_register, 1);
    outb(driveChanel[chanelIndex].controlPort                 , 0);


    ataSelectDrive(driveIndex);
    waitUntilDriveReady(driveIndex);


    // running identify command:
    outb(driveChanel[chanelIndex].basePort + DEVICE_REGISTERS_OFFSETS::command_register, ATA_CMD_IDENTIFY);
    ataIOWait(driveIndex);
    waitUntilDriveReady(driveIndex);


    
}

static void ataSelectDrive(DRIVE_NUMBER driveIndex)
{
    uint8_t drivePortForThisDrive = GET_CHANEL_PORT(driveIndex, basePort) + DEVICE_REGISTERS_OFFSETS::drive_register;
    uint8_t driveType = GET_DRIVE_TYPE(driveIndex);
    
    outb(drivePortForThisDrive, driveType);
}
static void waitUntilDriveReady(DRIVE_NUMBER driveIndex)
{
    uint8_t statusPortForThisDrive = GET_CHANEL_PORT(driveIndex, basePort) + DEVICE_REGISTERS_OFFSETS::status_register;
    while(inb(statusPortForThisDrive) & ATA_SR_BSY);
}
static void ataIOWait(DRIVE_NUMBER driveIndex)
{
    uint8_t alternateStatusPort = GET_CHANEL_PORT(driveIndex, controlPort) + ADJUST_DEVICE_OFFSET_TO_CORRECT_OFFSET(DEVICE_REGISTERS_OFFSETS::device_control_register);

    inb(alternateStatusPort);
	inb(alternateStatusPort);
	inb(alternateStatusPort);
	inb(alternateStatusPort);
}


/*#include "ideDefinitions.h"




static uint8_t ideIdentificationReadBuffer[IDE_BUFFER_SIZE] = {0};
static volatile uint8_t ideIrqInvoked = 0;
static uint8_t atapiPacket[ATAPI_COMMAND_BUFFER_SIZE] = {IDENTIFY_PACKED_DEVICE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


uint8_t ideRead(DRIVE_CHANEL_NUMBER chanelIndex, DEVICE_REGISTERS_OFFSETS regOffset)
{
    uint8_t result;

    if(regOffset > DEVICE_REGISTERS_OFFSETS::command_register && regOffset < DEVICE_REGISTERS_OFFSETS::altstatus_register)
    {
        ideWrite(chanelIndex, regOffset, ATA_REG_CONTROL_IRQ_ENABLE_MASK);
    }

    if     (regOffset < DEVICE_REGISTERS_OFFSETS::seccount1_register) { result = inb(masterAndSlaveChanels[chanelIndex].basePort + regOffset); }
    else if(regOffset < DEVICE_REGISTERS_OFFSETS::control_register  ) { result = inb(masterAndSlaveChanels[chanelIndex].basePort + regOffset - ) }
    return result;
}


void ideWrite(DRIVE_CHANEL_NUMBER chanelIndex, DEVICE_REGISTERS_OFFSETS regOffset, uint8_t data)
{

}


void ideInit()
{

}

static void ioWait()
{
    #define RECOMMENDED_TIME_TO_WAIT 420
    #define IO_PORT_ESTIMATED_EXECUTION_TIME 30

    for (uint8_t i = 0; i < RECOMMENDED_TIME_TO_WAIT / IO_PORT_ESTIMATED_EXECUTION_TIME; i++)
    {
        
    }

    #undef RECOMMENDED_TIME_TO_WAIT
    #undef IO_PORT_ESTIMATED_EXECUTION_TIME
}
*/