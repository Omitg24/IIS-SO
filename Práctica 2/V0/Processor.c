#include "Processor.h"
#include "ComputerSystem.h"
#include "Buses.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Local Functions prototypes
void Processor_FetchInstruction();
void Processor_DecodeAndExecuteInstruction();
void Processor_ManageInterrupts();
int Processor_DecodeOperationCode(BUSDATACELL);
int Processor_DecodeOperand1(BUSDATACELL);
int Processor_DecodeOperand2(BUSDATACELL);
void Processor_GetCodedInstruction(char * , BUSDATACELL);

// Processor registers
int registerPC_CPU; // Program counter
int registerAccumulator_CPU; // Accumulator
BUSDATACELL registerIR_CPU; // Instruction register
unsigned int registerPSW_CPU; // Processor state word
int registerMAR_CPU; // Memory Address Register
BUSDATACELL registerMBR_CPU; // Memory Buffer Register
int registerCTRL_CPU; // Control bus Register

enum Instruction
{
NONEXISTING_INST,
#define INST(name) name ## _INST, // name ## _INST concatena y define los enumerados ADD_INST y SUB_INST
#include "Instructions.def"
#undef INST
LAST_INST,
};

char *InstructionNames[] = {
"NONEXISTING_INSTRUCTION",
#define INST(name) #name, // #name Convierte el parametro name a String
#include "Instructions.def"
#undef INST
};


// Initialize processor registers
void Processor_InitializeRegisters(int regPC, int regAcum, unsigned int regPSW) {
	registerPC_CPU=regPC;
	registerAccumulator_CPU=regAcum;
	registerPSW_CPU=regPSW;
}

// This is the instruction cycle loop (fetch, decoding, execution, etc.).
// The processor stops working when an POWEROFF signal is stored in its
// PSW register
void Processor_InstructionCycleLoop() {

	while (registerPSW_CPU!=POWEROFF) {
		Processor_FetchInstruction();
		Processor_DecodeAndExecuteInstruction();
		Processor_ManageInterrupts();
	}
}

// Fetch an instruction from main memory and put it in the IR register
void Processor_FetchInstruction() {

	// The instruction must be located at the memory address pointed by the PC register
	registerMAR_CPU=registerPC_CPU;
	// Send to the main memory controller the address in which the reading has to take place: use the address bus for this
	Buses_write_AddressBus_From_To(CPU, MAINMEMORY);
	// Tell the main memory controller to read
	registerCTRL_CPU=CTRLREAD;
	Buses_write_ControlBus_From_To(CPU,MAINMEMORY);

	// All the read data is stored in the MBR register. Because it is an instruction
	// we have to copy it to the IR register
	memcpy((void *) (&registerIR_CPU), (void *) (&registerMBR_CPU), sizeof(BUSDATACELL));
	// Show initial part of HARDWARE message with Operation Code and operands
	char codedInstruction[13]; // Coded instruction with separated fields to show
	Processor_GetCodedInstruction(codedInstruction,registerIR_CPU);
	ComputerSystem_DebugMessage(1, HARDWARE, codedInstruction);

}	

// Decode and execute the instruction in the IR register
void Processor_DecodeAndExecuteInstruction() {
	// Decode
	int operationCode=Processor_DecodeOperationCode(registerIR_CPU);
	int operand1=Processor_DecodeOperand1(registerIR_CPU);
	int operand2=Processor_DecodeOperand2(registerIR_CPU);

	// Execute
	switch (operationCode) {
	  
		// Instruction ADD
		case ADD_INST:
			registerAccumulator_CPU= operand1 + operand2;
			registerPC_CPU++;
			break;
		
		// Instruction SHIFT (SAL and SAR)
		case SHIFT_INST: 
			operand1<0 ? (registerAccumulator_CPU <<= (-operand1)) : (registerAccumulator_CPU >>= operand1);
			registerPC_CPU++;
			break;
		
		// Instruction NOP
		case NOP_INST: 
			registerPC_CPU++;
			break;
			  
		// Instruction JUMP
		case JUMP_INST: 
			registerPC_CPU+= operand1;
			break;
			  
		// Instruction ZJUMP
		case ZJUMP_INST: 
			if (registerAccumulator_CPU==0)
				registerPC_CPU+= operand1;
			else
				registerPC_CPU++;
			break;

		// Instruction WRITE
		case WRITE_INST: 
			// Tell the main memory controller what
			registerMBR_CPU.cell= registerAccumulator_CPU;
			// Send to the main memory controller the data to be written: use the data bus for this
			Buses_write_DataBus_From_To(CPU, MAINMEMORY);
			// Tell the main memory controller where
			registerMAR_CPU=operand1;
			// Send to the main memory controller the address in which the writing has to take place: use the address bus for this
			Buses_write_AddressBus_From_To(CPU, MAINMEMORY);
			// Tell the main memory controller the operation
			registerCTRL_CPU=CTRLWRITE;
			// Send to the main memory controller the operation
			Buses_write_ControlBus_From_To(CPU,MAINMEMORY);

			registerPC_CPU++;
			break;

		// Instruction READ
		case READ_INST: 
			// Tell the main memory controller from where
			registerMAR_CPU=operand1;
			// Send to the main memory controller the address in which the reading has to take place: use the address bus for this
			Buses_write_AddressBus_From_To(CPU, MAINMEMORY);
			// Tell the main memory controller to read
			registerCTRL_CPU=CTRLREAD;
			// Send to the main memory controller the operation
			Buses_write_ControlBus_From_To(CPU,MAINMEMORY);

			// Copy the read data to the accumulator register
			registerAccumulator_CPU= registerMBR_CPU.cell;
			registerPC_CPU++;
			break;

		// Instruction INC
		case INC_INST: 
			registerAccumulator_CPU+= operand1;
			registerPC_CPU++;
			break;

		// Instruction HALT
		case HALT_INST: 
			registerPSW_CPU=POWEROFF;
			break;

		// Instruction MEMADD
		case MEMADD_INST:
			// Tell the main memory controller from where
			registerMAR_CPU=operand2;
			// Send to the main memory controller the address in which the reading has to take place: use the address bus for this
			Buses_write_AddressBus_From_To(CPU, MAINMEMORY);
			// Tell the main memory controller to read
			registerCTRL_CPU=CTRLREAD;
			// Send to the main memory controller the operation
			Buses_write_ControlBus_From_To(CPU,MAINMEMORY);

			registerAccumulator_CPU = operand1 + registerMBR_CPU.cell;
			break;
		// Unknown instruction
		default : registerPC_CPU++;
			  break;
	}
	// Show final part of HARDWARE message with	CPU registers
	ComputerSystem_DebugMessage(3,HARDWARE,InstructionNames[operationCode],operand1,operand2,registerPC_CPU,registerAccumulator_CPU,registerAccumulator_CPU,registerPSW_CPU);
}
	
// Hardware interrup processing
// Our primitive processor DOES NOT SUPPORT interrupts
void Processor_ManageInterrupts() {

}

// Getter for the registerMAR_CPU
int Processor_GetMAR() {
	return registerMAR_CPU;
}

// Setter for the registerMAR_CPU
void Processor_SetMAR(int data) {
	registerMAR_CPU=data;
}

// pseudo-getter for the registerMBR_CPU
void Processor_GetMBR(BUSDATACELL *toRegister) {
	memcpy((void*) toRegister, (void *) (&registerMBR_CPU), sizeof(BUSDATACELL));
}

// pseudo-setter for the registerMBR_CPU
void Processor_SetMBR(BUSDATACELL *fromRegister) {
	memcpy((void*) (&registerMBR_CPU), (void *) fromRegister, sizeof(BUSDATACELL));
}

// Getter for the registerCTRL_CPU
int Processor_GetCTRL() {
	// Only return last byte
	return registerCTRL_CPU & 0xf;
}

// Setter for the registerCTRL_CPU
void Processor_SetCTRL(int ctrl) {
  	registerCTRL_CPU |= (ctrl & 0xff);
}

int Processor_Encode(char opCode, int op1, int op2) {
	int mask=0x7ff; // binary: 0111 1111 1111 
	int sigOp1=op1<0;
	op1=sigOp1 ? ((-op1) & mask) : (op1 & mask);
	int sigOp2=op2<0;
	op2=sigOp2 ? ((-op2) & mask) : (op2 & mask);
	int cell=(opCode<<24);
	cell = cell | (sigOp1<<23) | (op1<<12);
	cell = cell | (sigOp2<<11) | op2;
	return cell;
}

int Processor_DecodeOperationCode(BUSDATACELL memCell) {
	// return InstructionCode[(memCell.cell>>24) & 0xff];
	return (memCell.cell>>24) & 0xff;
}

int Processor_DecodeOperand1(BUSDATACELL memCell) {
	int sigOp1=memCell.cell & (0x1<<23);
	int op1=(memCell.cell & (0x7ff<<12))>>12;
	op1=sigOp1?-op1:op1;
	return op1;
}

int Processor_DecodeOperand2(BUSDATACELL memCell) {
	int sigOp2= memCell.cell & (0x1<<11);
	int op2=(memCell.cell & (0x7ff));
	op2=sigOp2?-op2:op2;
	return op2;
}

void Processor_GetCodedInstruction(char * result, BUSDATACELL memCell){
	sprintf(result,"%02X %03X %03X",((registerIR_CPU.cell>>24)&0xff),((registerIR_CPU.cell>>12)&0xfff),(registerIR_CPU.cell&0xfff));
}

int Processor_ToInstruction(char * operation) {
	int i;
	for (i=0;i<LAST_INST;i++)
		if (strcasecmp(InstructionNames[i],operation)==0)
			return i;
	return NONEXISTING_INST;
}