import os

# text colors:
RED_ERROR_TEXT = '\033[91m'
RESET_TEXT_COLOR = '\033[0m'

SPECIFIC_INTERRUPTS_DIRECTORY_RELATIVE_TO_PROJECT = "project/src/os/src/interrupts/specificInterrupts"
ASM_FILE_CONTENT_TEMPLATE = """[bits 64]
%include "src/os/src/interrupts/specificInterrupts/popAndPushAll.inc"

[extern isr{}Handler]
asmIsr{}Handler:
  pushaq
  call isr{}Handler
  popaq

  sti
  iretq ; returning from the interrupt
  global asmIsr{}Handler
"""
HEADER_FILE_CONTENT_TEMPLATE = """#pragma once


/// @brief initializes the interrupt itself
void init{}Interrupt();


/// @brief the isr function that is being triggered when and interrupt occurs
extern "C" void isr{}Handler();
"""
CPP_FILE_CONTENT_TEMPLATE = """#include "{}.h"
#include "utility/utility.h"
#include "src/interrupts/idt.h"


void init{}Interrupt()
{{
    extern uint64_t asmIsr{}Handler;
    uint64_t asmIsr{}HandlerAddress = (uint64_t)(&asmIsr{}Handler);

    /* Example for the keyboard hardware interrupt 
    
    initSpecificInterrupt(
        asmIsrKeyboardHandlerAddress, NO_INTERRUPT_STACK_TABLE, 
        IDT_TA_InterruptGate, NO_INTERRUPT_EXTRA_BITS, (uint8_t)InterruptIndexes::keyboard
    );

    enableHardwareInterrupt(IRQ::keyboard); // only for hardware interrupts
    
    */
}}


void isr{}Handler()
{{
    // if it is hardware interrupt do not forget:
    // picSendEOI(IRQ::thisHardwareInterrupt);  
}}
"""


def createInterruptFile(interrupt_name : str, file_full_name : str, file_content : str):
    """function to create a new file if it does not exist and if it does then printing an error

    Args:
        interrupt_name (str): the name of interrupt this function is generating a file for
        file_full_name (str): the full name and path of the file that this function creates
        file_content (str): the content that will be in the file that this function creates
    """
    if not os.path.exists(file_full_name):
        with open(file_full_name, "w") as headerFile:
            headerFile.write(file_content)
    else:
        print(RED_ERROR_TEXT + "Error: some of the interrupt {} files already exists to run the script on this interrupt delete the files and rerun".format(interrupt_name) + RESET_TEXT_COLOR)


def createAllInterruptFiles(interrupt_name : str, fileDirectory : str):
    """function to auto generate the asm, header, and partially the cpp file of the interrupt

    Args:
        interrupt_name (str): the name of the interrupt that is being created
        fileDirectory (str): the directory that this files will be placed
    """
    # making the first char in the interrupt name capital to keep function name conventions:
    capital_interrupt_name = interrupt_name[0].capitalize() + interrupt_name[1:]

    # creating the interrupt directory in the case that it needs to be created:
    os.makedirs(fileDirectory, exist_ok = True)

    # creating the asm file:
    file_name = fileDirectory + "/" + interrupt_name + ".asm"
    file_content = ASM_FILE_CONTENT_TEMPLATE.format(capital_interrupt_name, capital_interrupt_name, capital_interrupt_name, capital_interrupt_name)
    createInterruptFile(interrupt_name, file_name, file_content)

    # creating the header file:
    file_name = fileDirectory + "/" + interrupt_name + ".h"
    file_content = HEADER_FILE_CONTENT_TEMPLATE.format(capital_interrupt_name, capital_interrupt_name)
    createInterruptFile(interrupt_name, file_name, file_content)

    # creating the cpp files:
    file_name = fileDirectory + "/" + interrupt_name + ".cpp"
    file_content = CPP_FILE_CONTENT_TEMPLATE.format(interrupt_name, capital_interrupt_name, capital_interrupt_name, capital_interrupt_name, capital_interrupt_name, capital_interrupt_name)
    createInterruptFile(interrupt_name, file_name, file_content)


def printMenu():
    """prints the menu of the options of interrupts the user can choose
    """
    print("what kind of interrupt do you want:")
    print("1. hardware interrupt")
    print("2. cpu triggered interrupt")
    print("3. software generated interrupt")


def takeInterruptTypeInput(inputMsg : str = "Enter your choice: ") -> int:
    """function that takes from the user the type of the interrupt that he wants

    Args:
        inputMsg (str, optional): the input msg for the user. Defaults to "Enter your choice: ".

    Returns:
        int: the interrupt option number
    """
    while True:
        try:
            number : int = int(input(inputMsg))
            
            # in the case that the input is not one of the choices
            if number < 1 or 3 < number:
                print(RED_ERROR_TEXT + "This is not one of the options pls enter an option between (1 - 3)" + RESET_TEXT_COLOR)
                continue

            return number
        
        except ValueError:
            print(RED_ERROR_TEXT + "Invalid input. Please enter a valid number." + RESET_TEXT_COLOR)


def getDirectoryOfInterrupt(interruptType : str, interruptName : str) -> str:
    """this functions is checking what is the directory that the auto generated files that will be created will be placed according to what the user
    of the function choose

    Args:
        interruptType (str): the type of interrupt 1 of 3 options
        interruptName (str): the name of the interrupt that is going to be created

    Returns:
        str: the path that the interrupt files will be placed in
    """
    project_directory : str = os.path.dirname(os.path.dirname(os.path.dirname(__file__)))
    interruptFilesDirectory : str = project_directory + "/"+ SPECIFIC_INTERRUPTS_DIRECTORY_RELATIVE_TO_PROJECT + "/" + interruptType + "/" + interruptName
    return interruptFilesDirectory


def main():
    user_wanted_interrupt_to_directory_to_place_files : dict = {
        1: "hardwareInterrupts",
        2: "cpuInterrupts",
        3: "softwareInterrupts"
    }

    # getting user inputs:
    printMenu()
    interrupt_type : str = user_wanted_interrupt_to_directory_to_place_files[takeInterruptTypeInput()]
    interrupt_name : str = str(input("Enter interrupt name: "))

    # getting the directory that the files will be auto generated into:
    directory_of_files : str = getDirectoryOfInterrupt(interrupt_type, interrupt_name)
    
    # creating the actual files that are needed
    createAllInterruptFiles(interrupt_name, directory_of_files)

if __name__ == "__main__":
    main()