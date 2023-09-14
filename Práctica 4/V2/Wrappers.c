#include "Clock.h"
#include "Asserts.h"

void __real_OperatingSystem_InterruptLogic(int);
int __real_Processor_FetchInstruction();
void __real_Processor_DecodeAndExecuteInstruction();
void __real_Processor_InstructionCycleLoop();

void __wrap_Processor_InstructionCycleLoop() {
	__real_Processor_InstructionCycleLoop();
	Asserts_TerminateAssertions();
}

int __wrap_Processor_FetchInstruction() {
	Clock_Update();
	return __real_Processor_FetchInstruction();
}

void __wrap_Processor_DecodeAndExecuteInstruction() {
	__real_Processor_DecodeAndExecuteInstruction();
	Asserts_CheckAsserts();
}

void __wrap_OperatingSystem_InterruptLogic(int entryPoint) {
	Clock_Update();
	__real_OperatingSystem_InterruptLogic(entryPoint);
}

