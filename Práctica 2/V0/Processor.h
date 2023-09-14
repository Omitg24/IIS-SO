#ifndef PROCESSOR_H
#define PROCESSOR_H

#define POWEROFF 1

#include "Buses.h"

// Functions prototypes
void Processor_InitializeRegisters(int, int, unsigned int);
void Processor_InstructionCycleLoop();
int Processor_Encode(char, int, int);
int Processor_ToInstruction(char *); 

// Buses needs to access MAR, MBR and CTRL
int Processor_GetMAR();
void Processor_SetMAR(int);
void Processor_GetMBR(BUSDATACELL *);
void Processor_SetMBR(BUSDATACELL *);
int Processor_GetCTRL();
void Processor_SetCTRL(int);

#endif
