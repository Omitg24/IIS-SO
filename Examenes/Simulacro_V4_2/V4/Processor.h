#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "MainMemory.h"
#include "ProcessorBase.h"

#define INTERRUPTTYPES 10
#define CPU_SUCCESS 1
#define CPU_FAIL 0

// Enumerated type that connects bit positions in the PSW register with
// processor events and status
//V2-EX3a-BEGIN
enum PSW_BITS {POWEROFF_BIT=0, ZERO_BIT=1, NEGATIVE_BIT=2, OVERFLOW_BIT=3, EXECUTION_MODE_BIT=7, INTERRUPT_MASKED_BIT=15};
//V2-EX3a-END

// Enumerated type that connects bit positions in the interruptLines with
// interrupt types 
//V2-EX2a-BEGIN
enum INT_BITS {SYSCALL_BIT=2, EXCEPTION_BIT=6, CLOCKINT_BIT=9};
//V2-EX2b-END

//V4-EX1b-BEGIN
enum EXCEPTIONS {DIVISIONBYZERO, INVALIDPROCESSORMODE, INVALIDADDRESS, INVALIDINSTRUCTION};
//V4-EX1b-END

//V4-EX1c-BEGIN
#define MULTIPLE_EXCEPTIONS
//V4-EX1c-END

// Functions prototypes
void Processor_InitializeInterruptVectorTable();
int Processor_FetchInstruction();
void Processor_InstructionCycleLoop();
void Processor_DecodeAndExecuteInstruction();
void Processor_RaiseInterrupt(const unsigned int);
void Processor_ManageInterrupts();
//V4-EX1a-BEGIN
int Processor_GetRegisterBCPU();
//V4-EX1b-END

char * Processor_ShowPSW();
int Processor_GetCTRL();
void Processor_SetCTRL(int);

#endif
