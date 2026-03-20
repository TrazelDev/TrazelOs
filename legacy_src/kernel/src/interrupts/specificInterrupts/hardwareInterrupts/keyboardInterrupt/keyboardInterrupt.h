#pragma once


/// @brief initializes the interrupt itself and sets up the function that is being called when an interrupt is being triggered
void initKeyboardInterrupt();


/// @brief the isr function that is being triggered when and interrupt occurs
extern "C" void isrKeyboardHandler();