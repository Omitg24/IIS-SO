#include "OperatingSystemBase.h"
#include "OperatingSystem.h"
#include "Processor.h"
#include "Buses.h"
#include "Clock.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Code that students should NOT touch

// Functions prototypes
int OperatingSystem_ObtainPositiveNumberOfFile(FILE *);
void OperatingSystem_PrepareDaemons(int);
void OperatingSystem_PrintSleepingProcessQueue();
void OperatingSystem_PrintExecutingProcessInformation();
void OperatingSystem_PrintProcessTableAssociation();

extern int initialPID;
extern int baseDaemonsInProgramList;

extern int executingProcessID;
#ifdef SLEEPINGQUEUE
	extern char * queueNames []; 
#endif
	
// Search for a free entry in the process table. The index of the selected entry
// will be used as the process identifier
int OperatingSystem_ObtainAnEntryInTheProcessTable() {

	int orig=initialPID%PROCESSTABLEMAXSIZE;
	int index=0;
	int entry;

	while (index<PROCESSTABLEMAXSIZE) {
		entry = (orig+index)%PROCESSTABLEMAXSIZE;
		if (processTable[entry].busy==0)
			return entry;
		else
			index++;
	}
	return NOFREEENTRY;
}


// Returns the size of the program, stored in the program file
int OperatingSystem_ObtainProgramSize(FILE *programFile) {
	int programSize;
	programSize = OperatingSystem_ObtainPositiveNumberOfFile(programFile);
	return programSize;
}

// Returns the priority of the program, stored in the program file
int OperatingSystem_ObtainPriority(FILE *programFile) {

	int processPriority;
	processPriority = OperatingSystem_ObtainPositiveNumberOfFile(programFile);
	return processPriority;
}


// Function that processes the contents of the file named by the first argument
// in order to load it in main memory from the address given as the second
// argument
// IT IS NOT NECESSARY TO COMPLETELY UNDERSTAND THIS FUNCTION

int OperatingSystem_LoadProgram(FILE *programFile, int initialAddress, int size) {

	char lineRead[MAXLINELENGTH];
	char *token0, *token1, *token2;
	BUSDATACELL data;
	int nbInstructions = 0;
	int opCode;
	int op1, op2;

	Processor_SetMAR(initialAddress);
	while (fgets(lineRead, MAXLINELENGTH, programFile) != NULL) {
		// REMARK: if lineRead is greater than MAXLINELENGTH in number of characters, the program
		// loading does not work
		opCode=op1=op2=0;
		token0=strtok(lineRead," \n\r\t");
		if (token0!=NULL && token0[0]!='/' && token0[0]!='\n' && token0[0]!='\r') {
			// I have an instruction with, at least, an operation code
			opCode=Processor_ToInstruction(token0);
			token1=strtok(NULL," ");
			if (token1!=NULL && token1[0]!='/') {
				// I have an instruction with, at least, an operand
			    op1=atoi(token1);
				token2=strtok(NULL," ");
				if (token2!=NULL && token2[0]!='/') {
					// The read line is similar to 'ADD 2 3 //coment'
					// I have an instruction with two operands
				  	op2=atoi(token2);
				}
			}
			
			// More instructions than size...
			if (++nbInstructions > size){
				return TOOBIGPROCESS;
			}

		    data.cell=Processor_Encode(opCode,op1,op2);
			Processor_SetMBR(&data);
			// Send data to main memory using the system buses
			Buses_write_DataBus_From_To(CPU, MAINMEMORY);
			Buses_write_AddressBus_From_To(CPU, MAINMEMORY);
			// Tell the main memory controller to write
			Processor_SetCTRL(CTRLWRITE);
			Buses_write_ControlBus_From_To(CPU,MAINMEMORY);
			Processor_SetMAR(Processor_GetMAR()+1);
		}
	}
	return SUCCESS;
}


// Auxiliar for check that line begins with positive number
int OperatingSystem_lineBeginsWithAPositiveNumber(char * line) {
	int i;
	
	for (i=0; i<strlen(line) && line[i]==' '; i++); // Don't consider blank spaces
	// If is there a digit number...
	if (i<strlen(line) && isdigit(line[i]))
		// It's a positive number
		return 1;
	else
		return 0;
}

int OperatingSystem_ObtainPositiveNumberOfFile(FILE *programFile) {
	char lineRead[MAXLINELENGTH];
	int isComment=1;
	int value=PROGRAMNOTVALID;

	// Read the first number as the size of the program. Skip all comments.
	while (isComment==1) {
		if (fgets(lineRead, MAXLINELENGTH, programFile) == NULL) {
		    return PROGRAMNOTVALID;
		}
		else
		    if (lineRead[0]!='/' && lineRead[0]!='\n' && lineRead[0]!='\r') { // Line IS NOT a comment and IS NOT empty
			    isComment=0;
			    if (OperatingSystem_lineBeginsWithAPositiveNumber(lineRead)) {
					value=atoi(strtok(lineRead," "));
				}
			    else {
					return PROGRAMNOTVALID;
				}
		    }
	}
	// Only sizes above 0 are allowed
	if (value<=0){
	    return PROGRAMNOTVALID;
	}
	
	return value;
}

// Daemon processes are system processes, that is, they work together with the OS.
// The System Idle Process uses the CPU whenever a user process is able to use it
void OperatingSystem_PrepareDaemons(int programListDaemonsBase) {
  
	// Include a entry for SystemIdleProcess at 0 position
	programList[0]=(PROGRAMS_DATA *) malloc(sizeof(PROGRAMS_DATA));

	programList[0]->executableName="SystemIdleProcess";
	programList[0]->arrivalTime=0;
	programList[0]->type=DAEMONPROGRAM; // daemon program

	sipID=initialPID%PROCESSTABLEMAXSIZE; // first PID for sipID

	// Preparing aditionals daemons here
	// index for aditionals daemons program in programList
	baseDaemonsInProgramList=OperatingSystem_PrepareTeachersDaemons(programListDaemonsBase,DAEMONS_PROGRAMS_FILE);

}

int OperatingSystem_PrepareTeachersDaemons(int programListDaemonsBase, char * daemonsProgramsFileName){
	FILE *daemonsFile;
	char lineRead[MAXLINELENGTH];
	PROGRAMS_DATA *progData;
	char *name, *arrivalTime;
	int time;
	int numberOfDaemons=0;

	// daemonsFile= fopen("teachersDaemons", "r");
	daemonsFile= fopen(daemonsProgramsFileName, "r");

	// Check if programFile exists
	if (daemonsFile==NULL)
		return programListDaemonsBase;

	while (fgets(lineRead, MAXLINELENGTH, daemonsFile) != NULL
					 && programListDaemonsBase<PROGRAMSMAXNUMBER) {
		name=strtok(lineRead,",");
	    if (name==NULL){
			continue;
		}

		arrivalTime=strtok(NULL,",");
    	if (arrivalTime==NULL
    		|| sscanf(arrivalTime,"%d",&time)==0)
    		time=0;
    	
    	progData=(PROGRAMS_DATA *) malloc(sizeof(PROGRAMS_DATA));
    	progData->executableName = (char *) malloc((strlen(name)+1)*sizeof(char));
    	strcpy(progData->executableName,name);
    	progData->arrivalTime=time;
    	progData->type=DAEMONPROGRAM;
    	programList[programListDaemonsBase++]=progData;
		numberOfDaemons++;
	}

	ComputerSystem_DebugMessage(77,POWERON,numberOfDaemons,daemonsProgramsFileName);

	return programListDaemonsBase;
}

void OperatingSystem_ReadyToShutdown(){
	// Simulation must finish (done by modifying the PC of the System Idle Process so it points to its 'TRAP 3' instruction,
	// located at the last memory position used by that process, and dispatching sipId (next ShortTermSheduled)
	processTable[sipID].copyOfPCRegister=processTable[sipID].initialPhysicalAddress+processTable[sipID].processSize-1;
}

void OperatingSystem_TerminatingSIP() {
	Processor_CopyInSystemStack(MAINMEMORYSIZE-1,OS_address_base+1);
	executingProcessID=NOPROCESS;
}

// Show time messages
void OperatingSystem_ShowTime(char section) {
	ComputerSystem_DebugMessage(100,section,Processor_PSW_BitState(EXECUTION_MODE_BIT)?"\t":"");
	ComputerSystem_DebugMessage(Processor_PSW_BitState(EXECUTION_MODE_BIT)?95:94,section,Clock_GetTime());
}

// Show general status
void OperatingSystem_PrintStatus(){ 
	OperatingSystem_PrintExecutingProcessInformation(); // Show executing process information
	OperatingSystem_PrintReadyToRunQueue();  // Show Ready to run queues implemented for students
	OperatingSystem_PrintSleepingProcessQueue(); // Show Sleeping process queue
	OperatingSystem_PrintProcessTableAssociation(); // Show PID-Program's name association

}

 // Show Executing process information
void OperatingSystem_PrintExecutingProcessInformation(){ 
#ifdef SLEEPINGQUEUE

	OperatingSystem_ShowTime(SHORTTERMSCHEDULE);
	if (executingProcessID>=0)
		// Show message "Running Process Information:\n\t\t[PID: executingProcessID, Priority: priority, WakeUp: whenToWakeUp, Queue: queueID]\n"
		ComputerSystem_DebugMessage(74,SHORTTERMSCHEDULE,
			executingProcessID,processTable[executingProcessID].priority,processTable[executingProcessID].whenToWakeUp
			,queueNames[processTable[executingProcessID].queueID]);
	else
		ComputerSystem_DebugMessage(100,SHORTTERMSCHEDULE,"Running Process Information:\n\t\t[--- No running process ---]\n");

#endif
}

// Show SleepingProcessQueue 
void OperatingSystem_PrintSleepingProcessQueue(){ 
#ifdef SLEEPINGQUEUE

	int i;
	OperatingSystem_ShowTime(SHORTTERMSCHEDULE);
	//  Show message "SLEEPING Queue:\n\t\t");
	ComputerSystem_DebugMessage(100,SHORTTERMSCHEDULE,"SLEEPING Queue:\n\t\t");
	if (numberOfSleepingProcesses>0)
		for (i=0; i< numberOfSleepingProcesses; i++) {
			// Show message [PID, priority, whenToWakeUp]
			ComputerSystem_DebugMessage(75,SHORTTERMSCHEDULE
				, sleepingProcessesQueue[i].info
				, processTable[sleepingProcessesQueue[i].info].priority
				, processTable[sleepingProcessesQueue[i].info].whenToWakeUp);
			if (i<numberOfSleepingProcesses-1)
	  			ComputerSystem_DebugMessage(100,SHORTTERMSCHEDULE,", ");
  		}
  	else 
	  	ComputerSystem_DebugMessage(100,SHORTTERMSCHEDULE,"[--- empty queue ---]");
  ComputerSystem_DebugMessage(100,SHORTTERMSCHEDULE,"\n");

#endif
}

void OperatingSystem_PrintProcessTableAssociation() {
  int i;
  OperatingSystem_ShowTime(SHORTTERMSCHEDULE);
  //  Show message "Process table association with program's name:");
  ComputerSystem_DebugMessage(100,SHORTTERMSCHEDULE,"PID association with program's name:\n");
  for (i=0; i< PROCESSTABLEMAXSIZE; i++) {
  	if (processTable[i].busy) {
  		// Show message PID -> program's name\n
  		ComputerSystem_DebugMessage(76,SHORTTERMSCHEDULE,i,programList[processTable[i].programListIndex]->executableName);
  	}
  }
}

