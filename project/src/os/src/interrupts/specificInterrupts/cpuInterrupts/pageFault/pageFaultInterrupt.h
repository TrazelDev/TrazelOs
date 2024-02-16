// this an auto generated file
#pragma once


/// @brief initializes the interrupt itself
void initPageFaultInterrupt();


/// @brief the isr function that is being triggered when and interrupt occurs
extern "C" void isrPageFaultHandler();
