#ifndef OPERATINGSYSTEM_H
#define OPERATINGSYSTEM_H

#include "ComputerSystem.h"
#include <stdio.h>


#define SUCCESS 1
#define PROGRAMDOESNOTEXIST -1
#define PROGRAMNOTVALID -2

// In this version, every process occupies a 60 positions main memory chunk 
// so we can use 60 positions for OS code and the system stack
#define MAINMEMORYSECTIONSIZE (MAINMEMORYSIZE / (PROCESSTABLEMAXSIZE+1))

#define NOFREEENTRY -3
#define TOOBIGPROCESS -4

#define NOPROCESS -1

// Contains the possible type of programs
enum ProgramTypes { USERPROGRAM, DAEMONPROGRAM }; 

// Enumerated type containing all the possible process states
enum ProcessStates { NEW, READY, EXECUTING, BLOCKED, EXIT};

// Enumerated type containing the list of system calls and their numeric identifiers
//V1-EX12-BEGIN
//V2-EX5d-BEGIN
enum SystemCallIdentifiers { SYSCALL_END=3, SYSCALL_YIELD=4, SYSCALL_PRINTEXECPID=5, SYSCALL_SLEEP = 7};
//V2-EX5d-END
//V1-EX12-END

//V1-EX11a-BEGIN
#define NUMBEROFQUEUES 2
enum TypeOfReadyToRunProcessQueues { USERPROCESSQUEUE, DAEMONSQUEUE};
//V1-EX11a-END

//V4-EX5-BEGIN
// Partitions configuration definition
#define MEMCONFIG // in OperatingSystem.h
//V4-EX5-END

// A PCB contains all of the information about a process that is needed by the OS
typedef struct {
	int busy;
	int initialPhysicalAddress;
	int processSize;
	int state;
	int priority;
	int copyOfPCRegister;
	unsigned int copyOfPSWRegister;
	int programListIndex;
	//V1-EX11a-BEGIN
	int queueID;
	//V1-EX11a-END
	//V1-EX13c-BEGIN
	int copyOfAcumulatorRegister;
	//V1-EX13c-END
	//V2-EX5a-BEGIN
	int whenToWakeUp;
	//V2-EX5b-END
	//V4-EX8-BEGIN
	int numberOfPartition;
	//V4-EX8-END
} PCB;

//V2-EX5a-BEGIN
#define SLEEPINGQUEUE
//V2-EX5b-END

//V4-EX6d-BEGIN
#define MEMORYFULL -5
//V4-EX6d-END

// These "extern" declaration enables other source code files to gain access
// to the variable listed

// Functions prototypes
void OperatingSystem_Initialize();
void OperatingSystem_InterruptLogic(int);
int OperatingSystem_PrepareStudentsDaemons(int);
//V3-EX2-BEGIN
int OperatingSystem_GetExecutingProcessID();
//V3-EX2-END

#endif
