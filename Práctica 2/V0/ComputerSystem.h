#ifndef COMPUTERSYSTEM_H
#define COMPUTERSYSTEM_H

#include "Simulator.h"

// Functions prototypes
void ComputerSystem_PowerOn(int argc, char *argv[]);
void ComputerSystem_PowerOff();
void ComputerSystem_DebugMessage(int , char,  ...);

// General parameters of the global Computer System
#define PROGRAM_TO_BE_EXECUTED "prog-V0-E4"

// Main memory size (number of memory cells)
#define MAINMEMORYSIZE 256


// Sections in which we divide our simulator in terms of
// debugging messages that show its internal working details

#define ALL 'a'		// Show all the messages corresponding to calls to the  ComputerSystem_DebugMessage() function
#define NONE 'n'	// Do not show any message (not apply for ERROR messages)
#define INTERRUPT 'i'	// Only messages related to interruptions
#define HARDWARE 'h'	// Only messages related to the hardware
#define SYSFILE 'f'	// Only messages related to the file system of the operating system
#define SYSPROC 'p'	// Only messages related to the process management system of the operating system
#define SYSMEM 'm'	// Only messages related to the memory management system of the operating system
#define INIT 't'	// Only messages related to the initialization of the operating system
#define SHUTDOWN 'd'		// Only messages related to the process of powering off the operating system
#define LONGTERMSCHEDULE 'l'	// Only messages related to the long term scheduler of the operating system
#define SHORTTERMSCHEDULE 's'	// Only messages related to the short term scheduler of the operating system
#define ERROR 'e' // Error messages 

#endif
