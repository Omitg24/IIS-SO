#ifndef WRAPPERS_H
#define WRAPPERS_H

void __wrap_Processor_InstructionCycleLoop();
int __wrap_Processor_FetchInstruction();
void __wrap_Processor_DecodeAndExecuteInstruction();
void __wrap_OperatingSystem_InterruptLogic(int);

#endif