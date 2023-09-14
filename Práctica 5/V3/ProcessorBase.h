#ifndef PROCESSORBASE_H
#define PROCESSORBASE_H

#include "Buses.h"

enum Instruction {
NONEXISTING_INST,
#define INST(name) name ## _INST, // name ## _INST concatena y define los enumerados ADD_INST y SUB_INST
#include "Instructions.def"
#undef INST
LAST_INST,
};

void Processor_UpdatePSW();
void Processor_CheckOverflow(int , int );

void Processor_CopyInSystemStack(int, int);
int Processor_CopyFromSystemStack(int);
void Processor_RaiseInterrupt(const unsigned int);
void Processor_ACKInterrupt(const unsigned int);
unsigned int Processor_GetInterruptLineStatus(const unsigned int);


void Processor_ActivatePSW_Bit(const unsigned int);
void Processor_DeactivatePSW_Bit(const unsigned int);
unsigned int Processor_PSW_BitState(const unsigned int);

// The OS needs to access MAR and MBR registers to save the context of
// the process to which the processor is being assigned
// Buses needs to access MAR and MBR
int Processor_GetMAR();
void Processor_SetMAR(int);
void Processor_GetMBR(BUSDATACELL *);
void Processor_SetMBR(BUSDATACELL *);

// The OS needs to access the accumulator register to restore the context of
// the process to which the processor is being assigned and to save the context
// of the process being preempted for another ready process
void Processor_SetAccumulator(int);
int Processor_GetAccumulator();

// The OS needs to access the PC register to restore the context of
// the process to which the processor is being assigned
void Processor_SetPC(int);

// The OS needs to access register A to when executing the system call management
// routine, so it will be able to know the invoked system call identifier
int Processor_GetRegisterA();

// The OS needs to access the PSW register to restore the context of
// the process to which the processor is being assigned
void Processor_SetPSW(unsigned int);
unsigned int Processor_GetPSW();

int Processor_Encode(int , int , int);
int Processor_DecodeOperationCode(BUSDATACELL);
int Processor_DecodeOperand1(BUSDATACELL);
int Processor_DecodeOperand2(BUSDATACELL);
void Processor_GetCodedInstruction(char * , BUSDATACELL );
int Processor_ToInstruction(char *); 

#endif
