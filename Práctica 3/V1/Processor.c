#include "Processor.h"
#include "ProcessorBase.h"
#include "OperatingSystem.h"
#include "OperatingSystemBase.h"
#include "Buses.h"
#include <stdio.h>
#include <string.h>
#include "Wrappers.h"

// Internals Functions prototypes
void Processor_ManageInterrupts();

// External data
extern char *InstructionNames[];

// Processor registers
int registerPC_CPU; // Program counter
int registerAccumulator_CPU; // Accumulator
BUSDATACELL registerIR_CPU; // Instruction register
unsigned int registerPSW_CPU = 128; // Processor state word, initially protected mode
int registerMAR_CPU; // Memory Address Register
BUSDATACELL registerMBR_CPU; // Memory Buffer Register
int registerCTRL_CPU; // Control bus Register

int registerA_CPU; // General purpose register

int interruptLines_CPU; // Processor interrupt lines

// interrupt vector table: an array of handle interrupt memory addresses routines  
int interruptVectorTable[INTERRUPTTYPES];

// For PSW show "--------X---FNZS"
char pswmask []="----------------"; 

// Initialization of the interrupt vector table
void Processor_InitializeInterruptVectorTable(int interruptVectorInitialAddress) {
	int i;
	for (i=0; i< INTERRUPTTYPES;i++) { // Inicialice all to inicial IRET
		interruptVectorTable[i]=interruptVectorInitialAddress-2;  
	}

	interruptVectorTable[SYSCALL_BIT]=interruptVectorInitialAddress;  // SYSCALL_BIT=2
	interruptVectorTable[EXCEPTION_BIT]=interruptVectorInitialAddress+2; // EXCEPTION_BIT=6
}

// Fetch an instruction from main memory and put it in the IR register
int Processor_FetchInstruction() {

	// The instruction must be located at the logical memory address pointed by the PC register
	registerMAR_CPU=registerPC_CPU;
	// Send to the MMU the address in which the reading has to take place: use the address bus for this
	Buses_write_AddressBus_From_To(CPU, MMU);
	// Tell the main memory controller to read
	registerCTRL_CPU=CTRLREAD;
	Buses_write_ControlBus_From_To(CPU,MMU);

	if (registerCTRL_CPU & CTRL_SUCCESS) {
		// All the read data is stored in the MBR register. Because it is an instruction
		// we have to copy it to the IR register
		memcpy((void *) (&registerIR_CPU), (void *) (&registerMBR_CPU), sizeof(BUSDATACELL));
		// Show initial part of HARDWARE message with Operation Code and operands
		// Show message: operationCode operand1 operand2
		char codedInstruction[13]; // Coded instruction with separated fields to show
		Processor_GetCodedInstruction(codedInstruction,registerIR_CPU);
		ComputerSystem_DebugMessage(68, HARDWARE, codedInstruction);
	}
	else {
		// Show message: "_ _ _ "
		ComputerSystem_DebugMessage(100,HARDWARE,"_ _ _\n");
		return CPU_FAIL;
	}
	return CPU_SUCCESS;
}


// Decode and execute the instruction in the IR register
void Processor_DecodeAndExecuteInstruction() {
	int tempAcc; // for save accumulator if necesary

	// Decode
	int operationCode=Processor_DecodeOperationCode(registerIR_CPU);
	int operand1=Processor_DecodeOperand1(registerIR_CPU);
	int operand2=Processor_DecodeOperand2(registerIR_CPU);

	Processor_DeactivatePSW_Bit(OVERFLOW_BIT);

	// Execute
	switch (operationCode) {
	  
		// Instruction ADD
		case ADD_INST:
			registerAccumulator_CPU= operand1 + operand2;
			Processor_CheckOverflow(operand1,operand2);
			registerPC_CPU++;
			break;
		
		// Instruction SHIFT (SAL and SAR)
		case SHIFT_INST: 
			  if (operand1<0) { // SAL do not allow more than 31 bists shift...
			  	if (registerAccumulator_CPU & (-1<<(sizeof(int)*8-((-operand1)&0x1f)))) // some bit overflow...
			  		Processor_ActivatePSW_Bit(OVERFLOW_BIT);
			  	registerAccumulator_CPU <<= ((-operand1) & 0x1f);// unnecesary & because Intel make this way...
			  } 
			  else	// SAR do not allow more than 31 bists shift...
			  	registerAccumulator_CPU >>= operand1 & 0x1f;// unnecesary & because Intel make this way...
			  
			  registerPC_CPU++;
			  break;
		
		// Instruction DIV
		case DIV_INST: 
			if (operand2 == 0)
				Processor_RaiseInterrupt(EXCEPTION_BIT); 
			else {
				registerAccumulator_CPU=operand1 / operand2;
				registerPC_CPU++;
			}
			break;
			  
		// Instruction TRAP
		case TRAP_INST: 
			Processor_RaiseInterrupt(SYSCALL_BIT);
			registerA_CPU=operand1;
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
		case ZJUMP_INST: // Jump if ZERO_BIT on
			if (Processor_PSW_BitState(ZERO_BIT))
				registerPC_CPU+= operand1;
			else
				registerPC_CPU++;
			break;

		// Instruction WRITE
		case WRITE_INST: 
			registerMBR_CPU.cell=registerAccumulator_CPU;
			registerMAR_CPU=operand1;
			// Send to the main memory controller the data to be written: use the data bus for this
			Buses_write_DataBus_From_To(CPU, MAINMEMORY);
			// Send to the MMU controller the address in which the writing has to take place: use the address bus for this
			Buses_write_AddressBus_From_To(CPU, MMU);
			// Tell the MMU controller to write
			registerCTRL_CPU=CTRLWRITE;
			Buses_write_ControlBus_From_To(CPU,MMU);
			registerPC_CPU++;
			break;

		// Instruction READ
		case READ_INST: 
			registerMAR_CPU=operand1;
			// Send to the MMU controller the address in which the reading has to take place: use the address bus for this
			Buses_write_AddressBus_From_To(CPU, MMU);
			// Tell the MMU controller to read
			registerCTRL_CPU=CTRLREAD;
			Buses_write_ControlBus_From_To(CPU,MMU);

			// Copy the read data to the accumulator register
			registerAccumulator_CPU= registerMBR_CPU.cell;
			registerPC_CPU++;
			break;

		// Instruction INC
		case INC_INST: 
			tempAcc=registerAccumulator_CPU;
			registerAccumulator_CPU += operand1;
			Processor_CheckOverflow(tempAcc,operand1);
			registerPC_CPU++;
			break;

		//V1-EX16a-BEGIN
		// Instruction HALT
		case HALT_INST: 
			if (Processor_PSW_BitState(EXECUTION_MODE_BIT)) {
				Processor_ActivatePSW_Bit(POWEROFF_BIT);
			} else {
				Processor_RaiseInterrupt(EXCEPTION_BIT);
			}			
			break;
			  
		// Instruction OS
		case OS_INST: // Make a operating system routine in entry point indicated by operand1
			if (Processor_PSW_BitState(EXECUTION_MODE_BIT)) {
				// Show final part of HARDWARE message with CPU registers
				// Show message: " (PC: registerPC_CPU, Accumulator: registerAccumulator_CPU, PSW: registerPSW_CPU [Processor_ShowPSW()]\n
				ComputerSystem_DebugMessage(69, HARDWARE,InstructionNames[operationCode],operand1,operand2,registerPC_CPU,registerAccumulator_CPU,registerPSW_CPU,Processor_ShowPSW());
				// Not all operating system code is executed in simulated processor, but really must do it... 
				OperatingSystem_InterruptLogic(operand1);
				registerPC_CPU++;
				// Update PSW bits (ZERO_BIT, NEGATIVE_BIT, ...)
				Processor_UpdatePSW();
				return; // Note: message show before... for operating system messages after...
			} else {
				Processor_RaiseInterrupt(EXCEPTION_BIT);
				return; // Note: message show before... for operating system messages after...
			}
		// Instruction IRET
		case IRET_INST: // Return from a interrupt handle manager call
			if (Processor_PSW_BitState(EXECUTION_MODE_BIT)) {
				registerPC_CPU=Processor_CopyFromSystemStack(MAINMEMORYSIZE-1);
				registerPSW_CPU=Processor_CopyFromSystemStack(MAINMEMORYSIZE-2);
			} else {
				Processor_RaiseInterrupt(EXCEPTION_BIT);
			}
			break;		
		//V1-EX16a-END		

		//V1-EX0-BEGIN
		case MEMADD_INST:
			// Tell the main memory controller from where
			registerMAR_CPU=operand2;
			// Send to the MMU the address in which the reading has to take place: use the address bus for this
			Buses_write_AddressBus_From_To(CPU, MMU);
			// Tell the main memory controller to read
			registerCTRL_CPU=CTRLREAD;
			// Send to the MMU the operation
			Buses_write_ControlBus_From_To(CPU,MMU);

			registerAccumulator_CPU = operand1 + registerMBR_CPU.cell;
			registerPC_CPU++;
			break;
		//V1-EX0-END

		// Unknown instruction
		default : 
			operationCode=NONEXISTING_INST;
			registerPC_CPU++;
			break;
	}
	
	// Update PSW bits (ZERO_BIT, NEGATIVE_BIT, ...)
	Processor_UpdatePSW();
	
	// Show final part of HARDWARE message with	CPU registers
	// Show message: " (PC: registerPC_CPU, Accumulator: registerAccumulator_CPU, PSW: registerPSW_CPU [Processor_ShowPSW()]\n
	ComputerSystem_DebugMessage(69, HARDWARE, InstructionNames[operationCode],operand1,operand2,registerPC_CPU,registerAccumulator_CPU,registerPSW_CPU,Processor_ShowPSW());
}
	
	
// Hardware interrupt processing
void Processor_ManageInterrupts() {
  
	int i;

		for (i=0;i<INTERRUPTTYPES;i++)
			// If an 'i'-type interrupt is pending
			if (Processor_GetInterruptLineStatus(i)) {
				// Deactivate interrupt
				Processor_ACKInterrupt(i);
				// Copy PC and PSW registers in the system stack
				Processor_CopyInSystemStack(MAINMEMORYSIZE-1, registerPC_CPU);
				Processor_CopyInSystemStack(MAINMEMORYSIZE-2, registerPSW_CPU);	
				// Activate protected excution mode
				Processor_ActivatePSW_Bit(EXECUTION_MODE_BIT);
				// Call the appropriate OS interrupt-handling routine setting PC register
				registerPC_CPU=interruptVectorTable[i];
				break; // Don't process another interrupt
			}
}

char * Processor_ShowPSW(){
	strcpy(pswmask,"----------------");
	int tam=strlen(pswmask)-1;
	if (Processor_PSW_BitState(EXECUTION_MODE_BIT))
		pswmask[tam-EXECUTION_MODE_BIT]='X';
	if (Processor_PSW_BitState(OVERFLOW_BIT))
		pswmask[tam-OVERFLOW_BIT]='F';
	if (Processor_PSW_BitState(NEGATIVE_BIT))
		pswmask[tam-NEGATIVE_BIT]='N';
	if (Processor_PSW_BitState(ZERO_BIT))
		pswmask[tam-ZERO_BIT]='Z';
	if (Processor_PSW_BitState(POWEROFF_BIT))
		pswmask[tam-POWEROFF_BIT]='S';
	return pswmask;
}


/////////////////////////////////////////////////////////
//  New functions below this line  //////////////////////
