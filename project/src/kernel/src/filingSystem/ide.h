// IDE - Integrated Drive Electronics
#pragma once
#include "utility/utility.h"

enum DRIVE_NUMBER
{
    DRIVE_MASTER_PRIMARY    = 0,
    DRIVE_SLAVE_PRIMARY     = 1,
    DRIVE_MASTER_SECONDARY  = 2,
    DRIVE_SLAVE_SECONDARY   = 3
};

/// @brief initializing the IDE controller
void ideInit(DRIVE_NUMBER driveIndex);

/// @brief selects a specific drive to use operation on
static void ataSelectDrive(DRIVE_NUMBER driveIndex);
/// @brief waits until the status register is clear so the next command can be send 
static void waitUntilDriveReady(DRIVE_NUMBER driveIndex);
/// @brief waits for the ata io operations to be safely accessible again
static void ataIOWait(DRIVE_NUMBER driveIndex);
