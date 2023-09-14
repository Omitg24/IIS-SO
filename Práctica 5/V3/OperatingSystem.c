#include "OperatingSystem.h"
#include "OperatingSystemBase.h"
#include "MMU.h"
#include "Processor.h"
#include "Buses.h"
#include "Heap.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

// Functions prototypes
void OperatingSystem_PCBInitialization(int, int, int, int, int);
void OperatingSystem_MoveToTheREADYState(int);
void OperatingSystem_Dispatch(int);
void OperatingSystem_RestoreContext(int);
void OperatingSystem_SaveContext(int);
void OperatingSystem_TerminateProcess();
int OperatingSystem_LongTermScheduler();
void OperatingSystem_PreemptRunningProcess();
int OperatingSystem_CreateProcess(int);
int OperatingSystem_ObtainMainMemory(int, int);
int OperatingSystem_ShortTermScheduler();
int OperatingSystem_ExtractFromReadyToRun(int);
void OperatingSystem_HandleException();
void OperatingSystem_HandleSystemCall();
//V1-EX9-BEGIN
void OperatingSystem_PrintReadyToRunQueue();
//V1-EX9-END
//V2-EX2b-BEGIN
void OperatingSystem_HandleClockInterrupt();
//V2-EX2b-END
//V2-EX5d-BEGIN
void OperatingSystem_MoveToTheBLOCKEDState(int);
//V2-EX5d-END
//V2-EX6a-BEGIN
int OperatingSystem_ExtractFromSleepingProcessesQueue();
//V2-EX6a-END
//V2-EX6c-BEGIN
int OperatingSystem_GetMostPriorityProcess();
//V2-EX6c-END
//V2-EX6d-BEGIN
void OperatingSystem_ChangeProcess();
//V2-EX6d-END

// The process table
PCB processTable[PROCESSTABLEMAXSIZE];

// Address base for OS code in this version
int OS_address_base = PROCESSTABLEMAXSIZE * MAINMEMORYSECTIONSIZE;

// Identifier of the current executing process
int executingProcessID=NOPROCESS;

// Identifier of the System Idle Process
int sipID;

// Initial PID for assignation
int initialPID=PROCESSTABLEMAXSIZE-1;

// Begin indes for daemons in programList
int baseDaemonsInProgramList; 

// Variable containing the number of not terminated user processes
int numberOfNotTerminatedUserProcesses=0;

char DAEMONS_PROGRAMS_FILE[MAXIMUMLENGTH]="teachersDaemons";

//V1-EX10a-BEGIN
char * statesNames [5]={"NEW","READY","EXECUTING","BLOCKED","EXIT"};
//V1-EX10a-END

// Array that contains the identifiers of the READY processes
//V1-EX11a-BEGIN
heapItem readyToRunQueue [NUMBEROFQUEUES][PROCESSTABLEMAXSIZE];
int numberOfReadyToRunProcesses[NUMBEROFQUEUES]={0,0};

char * queueNames [NUMBEROFQUEUES]={"USER","DAEMONS"};
//V1-EX11a-END

//V2-EX4-BEGIN
int numberOfClockInterrupts = 0;
//V2-EX4-END

//V2-EX5b-BEGIN
//Heap with blocked processes sort by when to wakeup
heapItem sleepingProcessesQueue[PROCESSTABLEMAXSIZE];
int numberOfSleepingProcesses = 0;
//V2-EX5b-END

// Initial set of tasks of the OS
void OperatingSystem_Initialize(int daemonsIndex) {
	
	int i, selectedProcess;
	//V1-EX15-BEGIN
	int numberOfSuccessfullyCreatedProcesses;
	//V1-EX15-END

	FILE *programFile; // For load Operating System Code
	programFile=fopen("OperatingSystemCode", "r");
	if (programFile==NULL){
		// Show red message "FATAL ERROR: Missing Operating System!\n"
		//V2-EX1-BEGIN
		OperatingSystem_ShowTime(SHUTDOWN);
		//V2-EX1-END
		ComputerSystem_DebugMessage(99,SHUTDOWN,"FATAL ERROR: Missing Operating System!\n");
		exit(1);		
	}

	// Obtain the memory requirements of the program
	int processSize=OperatingSystem_ObtainProgramSize(programFile);

	// Load Operating System Code
	OperatingSystem_LoadProgram(programFile, OS_address_base, processSize);
	
	// Process table initialization (all entries are free)
	for (i=0; i<PROCESSTABLEMAXSIZE;i++){
		processTable[i].busy=0;
	}
	// Initialization of the interrupt vector table of the processor
	Processor_InitializeInterruptVectorTable(OS_address_base+2);
		
	// Include in program list  all system daemon processes
	OperatingSystem_PrepareDaemons(daemonsIndex);
	
	//V3-EX1c-BEGIN
	ComputerSystem_FillInArrivalTimeQueue();
	//V3-EX1c-END
	//V3-EX1d-BEGIN
	OperatingSystem_PrintStatus();
	//V3-EX1d-END

	// Create all user processes from the information given in the command line
	//V1-EX15-BEGIN
	numberOfSuccessfullyCreatedProcesses = OperatingSystem_LongTermScheduler();

	//V3-EX5-BEGIN
	if (numberOfSuccessfullyCreatedProcesses <= 1 && OperatingSystem_IsThereANewProgram() == EMPTYQUEUE){
		OperatingSystem_ReadyToShutdown(); 
	}
	//V3-EX5-END
	//V1-EX15-END

	if (strcmp(programList[processTable[sipID].programListIndex]->executableName,"SystemIdleProcess")) {
		// Show red message "FATAL ERROR: Missing SIP program!\n"
		//V2-EX1-BEGIN
		OperatingSystem_ShowTime(SHUTDOWN);
		//V2-EX1-END
		ComputerSystem_DebugMessage(99,SHUTDOWN,"FATAL ERROR: Missing SIP program!\n");
		exit(1);		
	}

	// At least, one user process has been created
	// Select the first process that is going to use the processor
	selectedProcess=OperatingSystem_ShortTermScheduler();

	// Assign the processor to the selected process
	OperatingSystem_Dispatch(selectedProcess);

	// Initial operation for Operating System
	Processor_SetPC(OS_address_base);
}

// The LTS is responsible of the admission of new processes in the system.
// Initially, it creates a process from each program specified in the 
// 			command line and daemons programs
int OperatingSystem_LongTermScheduler() {
  
	int PID, i,
		numberOfSuccessfullyCreatedProcesses=0;

	i =0;
	//V3-EX3-BEGIN
	while (OperatingSystem_IsThereANewProgram() == YES) {
		i = Heap_poll(arrivalTimeQueue, QUEUE_ARRIVAL, &numberOfProgramsInArrivalTimeQueue);
	//V3-EX3-END
		PID=OperatingSystem_CreateProcess(i);
		switch (PID) {
			//V1-EX4b-BEGIN
			case NOFREEENTRY:
				//V2-EX1-BEGIN
				OperatingSystem_ShowTime(ERROR);
				//V2-EX1-END
				ComputerSystem_DebugMessage(103, ERROR, programList[i]->executableName);
				break;
			//V1-EX4b-END
			//V1-EX5a-BEGIN
			case PROGRAMDOESNOTEXIST:
				//V2-EX1-BEGIN
				OperatingSystem_ShowTime(ERROR);
				//V2-EX1-END
				ComputerSystem_DebugMessage(104, ERROR, programList[i]->executableName, "it does not exist");
				break;
			case PROGRAMNOTVALID:
				//V2-EX1-BEGIN
				OperatingSystem_ShowTime(ERROR);
				//V2-EX1-END
				ComputerSystem_DebugMessage(104, ERROR, programList[i]->executableName, "invalid priority or size");	
				break;
			//V1-EX5a-END
			//V1-EX6b-BEGIN
			case TOOBIGPROCESS:
				//V2-EX1-BEGIN
				OperatingSystem_ShowTime(ERROR);
				//V2-EX1-END
				ComputerSystem_DebugMessage(105, ERROR, programList[i]->executableName);	
				break;
			//V1-EX6b-END
		default:
			numberOfSuccessfullyCreatedProcesses++;
			if (programList[i]->type==USERPROGRAM) 
				numberOfNotTerminatedUserProcesses++;		
			// Move process to the ready state
			OperatingSystem_MoveToTheREADYState(PID);			
		}
	}
	//V2-EX7d-BEGIN
	if (numberOfSuccessfullyCreatedProcesses>0){
		OperatingSystem_PrintStatus();
	}	
	//V2-EX7d-END
	// Return the number of succesfully created processes
	return numberOfSuccessfullyCreatedProcesses;
}


// This function creates a process from an executable program
int OperatingSystem_CreateProcess(int indexOfExecutableProgram) {
  
	int PID;
	int processSize;
	int loadingPhysicalAddress;
	int priority;
	int instructions;
	FILE *programFile;
	PROGRAMS_DATA *executableProgram=programList[indexOfExecutableProgram];

	// Obtain a process ID
	PID=OperatingSystem_ObtainAnEntryInTheProcessTable();
	//V1-EX4a-BEGIN
	if (PID==NOFREEENTRY){
		return NOFREEENTRY;
	}
	//V1-EX4a-END
	// Check if programFile exists
	programFile=fopen(executableProgram->executableName, "r");

	//V1-EX5b-BEGIN
	if (programFile == NULL){
		return PROGRAMDOESNOTEXIST;
	}
	//V1-EX5b-END
	// Obtain the memory requirements of the program
	processSize=OperatingSystem_ObtainProgramSize(programFile);		
	// Obtain the priority for the process
	priority=OperatingSystem_ObtainPriority(programFile);	
	//V1-EX5c-END
	if (processSize == PROGRAMNOTVALID || priority == PROGRAMNOTVALID){
		return PROGRAMNOTVALID;
	}
	//V1-EX5c-END
	// Obtain enough memory space
 	loadingPhysicalAddress=OperatingSystem_ObtainMainMemory(processSize, PID);
	//V1-EX6a-BEGIN
	if (loadingPhysicalAddress==TOOBIGPROCESS){
		return TOOBIGPROCESS;
	}
	//V1-EX6a-END
	// Load program in the allocated memory
	//V1-EX7a-BEGIN
	instructions=OperatingSystem_LoadProgram(programFile, loadingPhysicalAddress, processSize);
	if (instructions==TOOBIGPROCESS){
		return TOOBIGPROCESS;
	}
	//V1-EX7a-END
	// PCB initialization	
	OperatingSystem_PCBInitialization(PID, loadingPhysicalAddress, processSize, priority, indexOfExecutableProgram);
	
	// Show message "Process [PID] created from program [executableName]\n"
	//V2-EX1-BEGIN
	OperatingSystem_ShowTime(INIT);
	//V2-EX1-END
	ComputerSystem_DebugMessage(70,INIT,PID,executableProgram->executableName);
	
	return PID;
	
}


// Main memory is assigned in chunks. All chunks are the same size. A process
// always obtains the chunk whose position in memory is equal to the processor identifier
int OperatingSystem_ObtainMainMemory(int processSize, int PID) {

 	if (processSize>MAINMEMORYSECTIONSIZE)
		return TOOBIGPROCESS;
	
 	return PID*MAINMEMORYSECTIONSIZE;
}


// Assign initial values to all fields inside the PCB
void OperatingSystem_PCBInitialization(int PID, int initialPhysicalAddress, int processSize, int priority, int processPLIndex) {

	processTable[PID].busy=1;
	processTable[PID].initialPhysicalAddress=initialPhysicalAddress;
	processTable[PID].processSize=processSize;
	processTable[PID].state=NEW;
	processTable[PID].priority=priority;
	processTable[PID].programListIndex=processPLIndex;
		
	// Daemons run in protected mode and MMU use real address
	if (programList[processPLIndex]->type == DAEMONPROGRAM) {
		processTable[PID].copyOfPCRegister=initialPhysicalAddress;
		processTable[PID].copyOfPSWRegister= ((unsigned int) 1) << EXECUTION_MODE_BIT;

		//V1-EX11c-BEGIN
		processTable[PID].queueID = DAEMONSQUEUE;
		//V1-EX11c-END
	} 
	else {
		processTable[PID].copyOfPCRegister=0;
		processTable[PID].copyOfPSWRegister=0;

		//V1-EX11c-BEGIN
		processTable[PID].queueID = USERPROCESSQUEUE;
		//V1-EX11c-END
	}

	//V1-EX10b-BEGIN	
	//V2-EX1-BEGIN
	OperatingSystem_ShowTime(SYSPROC);
	//V2-EX1-END
	ComputerSystem_DebugMessage(111, SYSPROC, PID, programList[processPLIndex]->executableName, statesNames[NEW]);
	//V1-EX10b-END
}


// Move a process to the READY state: it will be inserted, depending on its priority, in
// a queue of identifiers of READY processes
//V1-EX11c-BEGIN
void OperatingSystem_MoveToTheREADYState(int PID) {
	
	if (Heap_add(PID, readyToRunQueue[processTable[PID].queueID],QUEUE_PRIORITY ,&numberOfReadyToRunProcesses[processTable[PID].queueID] ,PROCESSTABLEMAXSIZE)>=0) {
		//V1-EX10b-BEGIN
		int previousState = processTable[PID].state;	
		processTable[PID].state=READY;
		//V2-EX1-BEGIN
		OperatingSystem_ShowTime(SYSPROC);
		//V2-EX1-END
		ComputerSystem_DebugMessage(110, SYSPROC, PID, programList[processTable[PID].programListIndex]->executableName, statesNames[previousState], statesNames[READY]);
		//V1-EX10b-END
		//V2-EX8-BEGIN
			//V1-EX9b-BEGIN		
			//OperatingSystem_PrintReadyToRunQueue();		
			//V1-EX9b-END
		//V2-EX8-END
	} 	
}
//V1-EX11c-END


// The STS is responsible of deciding which process to execute when specific events occur.
// It uses processes priorities to make the decission. Given that the READY queue is ordered
// depending on processes priority, the STS just selects the process in front of the READY queue
int OperatingSystem_ShortTermScheduler() {
	
	int selectedProcess = NOPROCESS;
	//V1-EX11c-BEGIN
	int i=0;
	for (i=0; i < NUMBEROFQUEUES && selectedProcess == NOPROCESS; i++) {
        selectedProcess=OperatingSystem_ExtractFromReadyToRun(i);
	}
	//V1-EX11c-END
	return selectedProcess;
}


// Return PID of more priority process in the READY queue
//V1-EX11c-BEGIN
int OperatingSystem_ExtractFromReadyToRun(int queueID) {
  
	int selectedProcess=NOPROCESS;
	
	selectedProcess=Heap_poll(readyToRunQueue[queueID],QUEUE_PRIORITY ,&numberOfReadyToRunProcesses[queueID]);
	
	// Return most priority process or NOPROCESS if empty queue
	return selectedProcess; 
}
//V1-EX11c-END

// Function that assigns the processor to a process
void OperatingSystem_Dispatch(int PID) {

	// The process identified by PID becomes the current executing process
	executingProcessID=PID;
	//V1-EX10b-BEGIN
	int previousState = processTable[PID].state;
	// Change the process' state
	processTable[PID].state=EXECUTING;	
	//V2-EX1-BEGIN
	OperatingSystem_ShowTime(SYSPROC);
	//V2-EX1-END
	ComputerSystem_DebugMessage(110, SYSPROC, PID, programList[processTable[PID].programListIndex]->executableName, statesNames[previousState], statesNames[EXECUTING]);
	//V1-EX10b-END	
	// Modify hardware registers with appropriate values for the process identified by PID
	OperatingSystem_RestoreContext(PID);
}


// Modify hardware registers with appropriate values for the process identified by PID
void OperatingSystem_RestoreContext(int PID) {
  
	// New values for the CPU registers are obtained from the PCB
	Processor_CopyInSystemStack(MAINMEMORYSIZE-1,processTable[PID].copyOfPCRegister);
	Processor_CopyInSystemStack(MAINMEMORYSIZE-2,processTable[PID].copyOfPSWRegister);
	//V1-EX13c-BEGIN
	Processor_SetAccumulator(processTable[PID].copyOfAcumulatorRegister);
	//V1-EX13c-END

	// Same thing for the MMU registers
	MMU_SetBase(processTable[PID].initialPhysicalAddress);
	MMU_SetLimit(processTable[PID].processSize);
}


// Function invoked when the executing process leaves the CPU 
void OperatingSystem_PreemptRunningProcess() {

	// Save in the process' PCB essential values stored in hardware registers and the system stack
	OperatingSystem_SaveContext(executingProcessID);
	// Change the process' state
	OperatingSystem_MoveToTheREADYState(executingProcessID);
	// The processor is not assigned until the OS selects another process
	executingProcessID=NOPROCESS;
}


// Save in the process' PCB essential values stored in hardware registers and the system stack
void OperatingSystem_SaveContext(int PID) {
	
	// Load PC saved for interrupt manager
	processTable[PID].copyOfPCRegister=Processor_CopyFromSystemStack(MAINMEMORYSIZE-1);
	
	// Load PSW saved for interrupt manager
	processTable[PID].copyOfPSWRegister=Processor_CopyFromSystemStack(MAINMEMORYSIZE-2);
	
	//V1-EX13c-BEGIN
	processTable[PID].copyOfAcumulatorRegister=Processor_GetAccumulator();
	//V1-EX13c-END
}


// Exception management routine
void OperatingSystem_HandleException() {
  
	// Show message "Process [executingProcessID] has generated an exception and is terminating\n"
	//V2-EX1-BEGIN
	OperatingSystem_ShowTime(SYSPROC);
	//V2-EX1-END
	ComputerSystem_DebugMessage(71,SYSPROC,executingProcessID,programList[processTable[executingProcessID].programListIndex]->executableName);
	
	OperatingSystem_TerminateProcess();
	//V2-EX7c-BEGIN
	OperatingSystem_PrintStatus();
	//V2-EX7c-END
}


// All tasks regarding the removal of the process
void OperatingSystem_TerminateProcess() {
  
	int selectedProcess;
	
	//V1-EX10b-BEGIN
  	int previousState = processTable[executingProcessID].state;
	processTable[executingProcessID].state=EXIT;
	//V2-EX1-BEGIN
	OperatingSystem_ShowTime(SYSPROC);
	//V2-EX1-END
	ComputerSystem_DebugMessage(110, SYSPROC, executingProcessID, programList[processTable[executingProcessID].programListIndex]->executableName, statesNames[previousState], statesNames[EXIT]);
	//V1-EX10b-END

	if (programList[processTable[executingProcessID].programListIndex]->type==USERPROGRAM) 
		// One more user process that has terminated
		numberOfNotTerminatedUserProcesses--;
		
	//V3-EX5-BEGIN
	if (numberOfNotTerminatedUserProcesses == 0 && OperatingSystem_IsThereANewProgram()==EMPTYQUEUE) {
	//V3-EX5-END
		if (executingProcessID==sipID) {
			// finishing sipID, change PC to address of OS HALT instruction
			OperatingSystem_TerminatingSIP();
			//V2-EX1-BEGIN
			OperatingSystem_ShowTime(SHUTDOWN);
			//V2-EX1-END
			ComputerSystem_DebugMessage(99,SHUTDOWN,"The system will shut down now...\n");
			return; // Don't dispatch any process
		}
		// Simulation must finish, telling sipID to finish
		OperatingSystem_ReadyToShutdown();
	}
	// Select the next process to execute (sipID if no more user processes)
	selectedProcess=OperatingSystem_ShortTermScheduler();

	// Assign the processor to that process
	OperatingSystem_Dispatch(selectedProcess);
}

// System call management routine
void OperatingSystem_HandleSystemCall() {  

	int systemCallID;
	int previousProcessID;
	int newProcessID;

	// Register A contains the identifier of the issued system call
	systemCallID=Processor_GetRegisterA();
	
	switch (systemCallID) {
		case SYSCALL_PRINTEXECPID:
			// Show message: "Process [executingProcessID] has the processor assigned\n"
			//V2-EX1-BEGIN
			OperatingSystem_ShowTime(SYSPROC);
			//V2-EX1-END
			ComputerSystem_DebugMessage(72,SYSPROC,executingProcessID,programList[processTable[executingProcessID].programListIndex]->executableName);
			break;

		//V1-EX12-BEGIN
		case SYSCALL_YIELD:
			if(numberOfReadyToRunProcesses[processTable[executingProcessID].queueID] > 0 && processTable[readyToRunQueue[processTable[executingProcessID].queueID][0].info].priority == processTable[executingProcessID].priority) {
				previousProcessID = executingProcessID;
				newProcessID = readyToRunQueue[processTable[executingProcessID].queueID][0].info;				

				//V2-EX1-BEGIN
				OperatingSystem_ShowTime(SHORTTERMSCHEDULE);
				//V2-EX1-END
				ComputerSystem_DebugMessage(115, SHORTTERMSCHEDULE, previousProcessID, programList[processTable[previousProcessID].programListIndex]->executableName, newProcessID, programList[processTable[newProcessID].programListIndex]->executableName);

				OperatingSystem_PreemptRunningProcess();
				OperatingSystem_Dispatch(OperatingSystem_ShortTermScheduler());
				//V2-EX7a-BEGIN
				OperatingSystem_PrintStatus();
				//V2-EX7a-END
			}
			break;
		//V1-EX12-END

		case SYSCALL_END:
			// Show message: "Process [executingProcessID] has requested to terminate\n"
			//V2-EX1-BEGIN
			OperatingSystem_ShowTime(SYSPROC);
			//V2-EX1-END
			ComputerSystem_DebugMessage(73,SYSPROC,executingProcessID,programList[processTable[executingProcessID].programListIndex]->executableName);
			OperatingSystem_TerminateProcess();
			//V2-EX7b-BEGIN
			OperatingSystem_PrintStatus();
			//V2-EX7b-END
			break;

		//V2-EX5d-BEGIN
		case SYSCALL_SLEEP:						
			OperatingSystem_MoveToTheBLOCKEDState(executingProcessID);
			OperatingSystem_Dispatch(OperatingSystem_ShortTermScheduler());
			//V2-EX5e-BEGIN
			OperatingSystem_PrintStatus();
			//V2-EX5e-END
			break;
		//V2-EX5d-END
	}
}

//V2-EX5d-BEGIN
void OperatingSystem_MoveToTheBLOCKEDState(int PID) {
	processTable[PID].whenToWakeUp = abs(Processor_GetAccumulator()) + numberOfClockInterrupts + 1;		
	if (Heap_add(PID, sleepingProcessesQueue, QUEUE_WAKEUP , &numberOfSleepingProcesses, PROCESSTABLEMAXSIZE)>=0) {		
		OperatingSystem_SaveContext(executingProcessID);
		int previousState = processTable[PID].state;				
		processTable[PID].state=BLOCKED;						
		OperatingSystem_ShowTime(SYSPROC);
		ComputerSystem_DebugMessage(110, SYSPROC, PID, programList[processTable[PID].programListIndex]->executableName, statesNames[previousState], statesNames[BLOCKED]);		
	}
}
//V2-EX5d-END

//	Implement interrupt logic calling appropriate interrupt handle
void OperatingSystem_InterruptLogic(int entryPoint){
	switch (entryPoint){
		case SYSCALL_BIT: // SYSCALL_BIT=2
			OperatingSystem_HandleSystemCall();
			break;
		case EXCEPTION_BIT: // EXCEPTION_BIT=6
			OperatingSystem_HandleException();
			break;
		//V2-EX2c-BEGIN
		case CLOCKINT_BIT:
			OperatingSystem_HandleClockInterrupt();
			break;
		//V2-EX2c-END
	}	
}

//V1-EX9a-BEGIN
void OperatingSystem_PrintReadyToRunQueue(){
	//V2-EX1-BEGIN
	OperatingSystem_ShowTime(SHORTTERMSCHEDULE);
	//V2-EX1-END
	ComputerSystem_DebugMessage(106, SHORTTERMSCHEDULE);
	//V1-EX11b-BEGIN	
	
	ComputerSystem_DebugMessage(112, SHORTTERMSCHEDULE, queueNames[USERPROCESSQUEUE]);
	for (int i = 0; i < numberOfReadyToRunProcesses[USERPROCESSQUEUE]; i++){
		if (i == numberOfReadyToRunProcesses[USERPROCESSQUEUE] - 1){
			ComputerSystem_DebugMessage(108, SHORTTERMSCHEDULE, readyToRunQueue[USERPROCESSQUEUE][i].info, processTable[readyToRunQueue[USERPROCESSQUEUE][i].info].priority);
		} else {
			ComputerSystem_DebugMessage(107, SHORTTERMSCHEDULE, readyToRunQueue[USERPROCESSQUEUE][i].info, processTable[readyToRunQueue[USERPROCESSQUEUE][i].info].priority);
		}		
	}
	if (numberOfReadyToRunProcesses[USERPROCESSQUEUE]==0){
		ComputerSystem_DebugMessage(109, SHORTTERMSCHEDULE);
	}
	ComputerSystem_DebugMessage(112, SHORTTERMSCHEDULE, queueNames[DAEMONSQUEUE]);
	for (int i = 0; i < numberOfReadyToRunProcesses[DAEMONSQUEUE]; i++){
		if (i == numberOfReadyToRunProcesses[DAEMONSQUEUE] - 1){
			ComputerSystem_DebugMessage(108, SHORTTERMSCHEDULE, readyToRunQueue[DAEMONSQUEUE][i].info, processTable[readyToRunQueue[DAEMONSQUEUE][i].info].priority);
		} else {
			ComputerSystem_DebugMessage(107, SHORTTERMSCHEDULE, readyToRunQueue[DAEMONSQUEUE][i].info, processTable[readyToRunQueue[DAEMONSQUEUE][i].info].priority);
		}		
	}
	if (numberOfReadyToRunProcesses[DAEMONSQUEUE]==0){
		ComputerSystem_DebugMessage(109, SHORTTERMSCHEDULE);
	}
	//V1-EX11b-END
}
//V1-EX9a-END

//V2-EX2b-BEGIN
void OperatingSystem_HandleClockInterrupt(){ 

	int processID;
	int sleepingProcessWhenToWakeUp;
	int numberOfUnblockedProcesses = 0;
	//V2-EX4-BEGIN
	numberOfClockInterrupts++;
	OperatingSystem_ShowTime(INTERRUPT);
	ComputerSystem_DebugMessage(120, INTERRUPT, numberOfClockInterrupts);
	//V2-EX4-END
	//V2-EX6a-BEGIN
	sleepingProcessWhenToWakeUp=processTable[Heap_getFirst(sleepingProcessesQueue, numberOfSleepingProcesses)].whenToWakeUp;
	while (numberOfSleepingProcesses>0 && sleepingProcessWhenToWakeUp==numberOfClockInterrupts){
		OperatingSystem_MoveToTheREADYState(OperatingSystem_ExtractFromSleepingProcessesQueue());			
		numberOfUnblockedProcesses++;
		//V3-EX4a-BEGIN
		OperatingSystem_LongTermScheduler();
		//V3-EX4a-END
	}	
	
	//V2-EX6a-END
	//V2-EX6b-BEGIN
	//V3-EX4b-BEGIN
	if (numberOfUnblockedProcesses>0 || OperatingSystem_LongTermScheduler()>=1){
	//V3-EX4b-END
		OperatingSystem_PrintStatus();				
		//V2-EX6c-BEGIN
		processID = OperatingSystem_GetMostPriorityProcess();
		if (processID != NOPROCESS){
			if (processTable[executingProcessID].queueID == processTable[processID].queueID) {
				if (processTable[executingProcessID].priority > processTable[processID].priority ){
					OperatingSystem_ChangeProcess(processID);
				}	
			} else if (processTable[executingProcessID].queueID == DAEMONSQUEUE && processTable[processID].queueID == USERPROCESSQUEUE){
				OperatingSystem_ChangeProcess(processID);
			}			
		}		
		//V2-EX6c-END	
	//V3-EX5-BEGIN
	} else if (numberOfUnblockedProcesses == 0 && numberOfNotTerminatedUserProcesses == 0 && OperatingSystem_IsThereANewProgram()==EMPTYQUEUE){
		OperatingSystem_ReadyToShutdown();
	}
	//V3-EX5-END
	//V2-EX6b-END		
}
//V2-EX2b-END

//V2-EX6a-BEGIN
int OperatingSystem_ExtractFromSleepingProcessesQueue() {  

	int selectedProcess=NOPROCESS;
	
	selectedProcess=Heap_poll(sleepingProcessesQueue, QUEUE_WAKEUP , &numberOfSleepingProcesses);

	return selectedProcess; 
}
//V2-EX6a-END

//V2-EX6c-BEGIN
int OperatingSystem_GetMostPriorityProcess() {
	
	int maxProcessID;

	maxProcessID = Heap_getFirst(readyToRunQueue[USERPROCESSQUEUE], numberOfReadyToRunProcesses[USERPROCESSQUEUE]);
	if (maxProcessID == NOPROCESS){
		maxProcessID = Heap_getFirst(readyToRunQueue[DAEMONSQUEUE], numberOfReadyToRunProcesses[DAEMONSQUEUE]);
	}	
	return maxProcessID;
}
//V2-EX6c-END

//V2-EX6d-BEGIN
void OperatingSystem_ChangeProcess(int processID){
	OperatingSystem_ShowTime(SHORTTERMSCHEDULE);
	ComputerSystem_DebugMessage(121, SHORTTERMSCHEDULE, executingProcessID, programList[processTable[executingProcessID].programListIndex]->executableName, processID, programList[processTable[processID].programListIndex]->executableName);

	OperatingSystem_PreemptRunningProcess();
	OperatingSystem_Dispatch(OperatingSystem_ShortTermScheduler());

	//V2-EX6d-BEGIN
	OperatingSystem_PrintStatus();
	//V2-EX6d-END
}
//V2-EX6d-END

//V3-EX2-BEGIN
int OperatingSystem_GetExecutingProcessID(){
	return executingProcessID;
}
//V2-EX2-END