#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Asserts.h"
#include "MainMemory.h"
#include "Clock.h"
#include "ComputerSystemBase.h"
#include "MMU.h"
#include "Heap.h"
#include "Processor.h"
#include "OperatingSystemBase.h"

extern MEMORYCELL mainMemory[];
extern int registerPC_CPU; // Program counter
extern int registerAccumulator_CPU; // Accumulator
extern BUSDATACELL registerIR_CPU; // Instruction register
extern unsigned int registerPSW_CPU; // Processor state word
extern int registerMAR_CPU; // Memory Address Register
extern BUSDATACELL registerMBR_CPU; // Memory Buffer Register
extern int executingProcessID; // Executing process PID
extern char *InstructionNames[]; // Names of processor instructions

char *elements[]={
	"RMEM_OP",	// Relative MEMory OPeration code
	"RMEM_O1",	// Relative MEMory Operand 1
	"RMEM_O2",	// Relative MEMory Operand 2
	"AMEM_OP",	// Absolute MEMory OPeration code
	"AMEM_O1",	// Absolute MEMory Operand 1
	"AMEM_O2",	// Absolute MEMory Operand 2
	"PC",		// Program Counter
	"ACC",		// ACCumulator
	"IR_OP",	// Instruction Register OPeration code
	"IR_O1",	// Instruction Register Operand 1
	"IR_O2",	// Instruction Register Operand 2
	"PSW",		// Processor State Word
	"MAR",		// Memory Address Register
	"MBR_OP",	// Memory Buffer Register OPeration code
	"MBR_O1",	// Memory Buffer Register Operand 1
	"MBR_O2",	// Memory Buffer Register Operand 2
	"MMU_BS",	// Memory Management Unit BaSe
	"MMU_LM",	// Memory Management Unit LiMit
	"MMU_MAR",	// Memory Management Unit Memory Address Register
	"MMEM_MAR",	// Main MEMory Memory Address Register
	"MMBR_OP",	// Main Memory Buffer Register OPeration code
	"MMBR_O1",	// Main Memory Buffer Register Operand 1
	"MMBR_O2",	// Main Memory Buffer Register Operand 2
	"XPID",		// eXecuting PID
	"RMEM",	// Relative MEMory
	"AMEM",	// Absolute MEMory
	"MBR",	// Memory Buffer Register
	"MMBR",	// Main Memory Buffer Register
	"PCB_ST",	// Process Table item state field
	"PCB_PC",	// Process Table item copyOfPCRegister field
	"PCB_PR",	// Process Table item priority field
	NULL};

ASSERT_DATA * asserts;
int MAX_ASSERTS=500; // Default number of asserts

// asserts heap
heapItem * assertsQueue;
int numOfElementsInAssertsQueue=0;

// All time asserts list is at the end of assersQueue in reverse order
int beginOfAllTimeAsserts;

// prototype functions
int Asserts_IsThereANewAssert(int);
void Asserts_CheckOneAssert(int);

int GEN_ASSERTS=0;

char ASSERTS_FILE[MAXIMUMLENGTH]="asserts";  // Default asserts file name

// Search assert element linearly into the array
int elementNumber(char *cmp) {
 int n=0;

 while ((elements[n]!=NULL) && strcmp(cmp, elements[n]))
   n++;

 if (elements[n]==NULL) return -1;
 else return n;
	
}

void strcpySpaces(char *target, char *src, int nChars) {
	int l, s, t;

	l=strlen(src);
	t=0;
	for (s=0; (t<nChars-1) && (s<l); s++) {
		if (src[s]!=' ' && src[s]!='\n' && src[s]!='\r')
			target[t++]=src[s];
	}
	target[t]=0;
}

int Asserts_LoadAsserts() {
	// load asserts file into asserts array;
 ASSERT_DATA a;

 char lineRead[MAXIMUMLENGTH];
 FILE *mf;
 char *time, *element, *value, *address;
 char svalue[E_SIZE];
 

 int lineNumber=0;;
 int numberAsserts=0;
 int rc;
 int en;

// All time asserts list is at the end of assersQueue in reverse order
 beginOfAllTimeAsserts=MAX_ASSERTS;

 mf=fopen(ASSERTS_FILE, "r");
 if (mf==NULL) {
	ComputerSystem_DebugMessage(82,POWERON,0);
	return -1;
  }
 
 // Array of asserts
 asserts =(ASSERT_DATA *) malloc(MAX_ASSERTS*sizeof(ASSERT_DATA));

 // Heap of index of asserts
 assertsQueue = (heapItem *) malloc (MAX_ASSERTS*sizeof(heapItem));

 ComputerSystem_DebugMessage(81, POWERON, ASSERTS_FILE, MAX_ASSERTS);
 
    
   while (fgets(lineRead,MAXIMUMLENGTH, mf) != NULL && numberAsserts<MAX_ASSERTS) {
	lineNumber++;	
	// reading up to 4 items for assert
	time=strtok(lineRead,",");
    if (time==NULL){
 		// printf("Illegal Assert in line %d of file %s\n",lineNumber,ASSERTS_FILE);
		ComputerSystem_DebugMessage(84,POWERON,lineNumber,ASSERTS_FILE);
		continue;
	}
	
	if ((time[0]=='/') || (time[0]=='\n')|| (time[0]=='\r'))
		continue; // Skip coments and empty lines

	element=strtok(NULL,",");
    if (element==NULL){
 		// printf("Illegal Assert in line %d of file %s\n",lineNumber,ASSERTS_FILE);
		ComputerSystem_DebugMessage(84,POWERON,lineNumber,ASSERTS_FILE);
		continue;
	}

	value=strtok(NULL,",");
    if (value==NULL){
 		// printf("Illegal Assert in line %d of file %s\n",lineNumber,ASSERTS_FILE);
		ComputerSystem_DebugMessage(84,POWERON,lineNumber,ASSERTS_FILE);
		continue;
	}
    address=strtok(NULL,"\n");

 	strcpySpaces(a.element, element,E_SIZE);
	if (strcmp(time,"*")) {
      rc=sscanf(time,"%d",&a.time);
	  if (rc==0){
 		// printf("Illegal time format in line %d of file %s\n",lineNumber,ASSERTS_FILE);
		ComputerSystem_DebugMessage(82,POWERON,lineNumber,ASSERTS_FILE);
		continue;
	  }
	}
    else a.time=-33; // All the instants of time.
        
	// If an Operation code read string (RMEM_OP, AMEM_OP,IR_OP, MBR_OP, MMBR_OP)
	en=elementNumber(a.element);
	if ((en==RMEM_OP) || (en==AMEM_OP) || (en==IR_OP) || (en==MBR_OP) || (en==MMBR_OP)) {
		strcpySpaces(svalue, value,E_SIZE);
		a.value=Processor_ToInstruction(svalue);
		if (a.value<0)
			rc=0;
	}
	else
		rc=sscanf(value,"%d",&a.value);

    if (rc==0){
 			// printf("Illegal expected value format in line %d of file %s (%s)\n",lineNumber,ASSERTS_FILE,value);
			ComputerSystem_DebugMessage(86,POWERON,lineNumber,ASSERTS_FILE, value);
			continue;
	}

    // If memory cell we read address (RMEM_OP, RMEM_O1, RMEM_O2, AMEM_OP, AMEM_O1, AMEM_O2)
	en=elementNumber(a.element);
	if ((en==RMEM_OP) || (en==RMEM_O1) || (en==RMEM_O2) 
		|| (en==AMEM_OP) || (en==AMEM_O1) || (en==AMEM_O2) 
		|| (en==RMEM) || (en==AMEM)
		|| (en==PCB_ST) || (en==PCB_PC) || (en==PCB_PR)) {
		if (address==NULL){
 			// printf("Illegal Assert in line %d of file %s\n",lineNumber,ASSERTS_FILE);
			ComputerSystem_DebugMessage(84,POWERON,lineNumber,ASSERTS_FILE);
			continue;
		}
		rc=sscanf(address,"%d",&a.address);
        if (rc==0){
	 		// printf("Illegal address format in line %d of file %s\n",lineNumber,ASSERTS_FILE);
			ComputerSystem_DebugMessage(87,POWERON,lineNumber,ASSERTS_FILE);
			continue;    
	    }
	}
	else a.address=0;

	asserts[numberAsserts]=a;

	if (a.time!=-33)
		// add to asserts heap
		Heap_add(numberAsserts,assertsQueue,QUEUE_ASSERTS,&numOfElementsInAssertsQueue,MAX_ASSERTS);
	else
		// add at the end and update starting point of all time asserts
		assertsQueue[--beginOfAllTimeAsserts].info=numberAsserts;

	numberAsserts++;
   }
   fclose(mf);
   if (numberAsserts==MAX_ASSERTS)
		// printf("Warning maximun number of asserts reached !!!  (%d)\n",numberAsserts);
		ComputerSystem_DebugMessage(83,POWERON,lineNumber,ASSERTS_FILE);
   else
	   asserts[numberAsserts].time=-1; // not necesary...

   ComputerSystem_DebugMessage(82,POWERON,numberAsserts);

   return numberAsserts;
}


void genAssertMsg(int time, char *ele, int realValue, int addr) {
	int en;

	printf("%d, %s",time, ele);
	en=elementNumber(ele);
	
	if ((en==RMEM_OP) || (en==AMEM_OP) || (en==IR_OP) || (en==MBR_OP)) 
	  	printf(", %s",InstructionNames[realValue]);
	else
		printf(", %d", realValue);
	
	if ((en==RMEM_OP) || (en==RMEM_O1) || (en==RMEM_O2)
		 || (en==AMEM_OP) || (en==AMEM_O1) || (en==AMEM_O2) 
		 || (en==RMEM) || (en==AMEM)
		 || (en==PCB_ST) || (en==PCB_PC) || (en==PCB_PR))
		printf(", %d", addr);
	
	printf("\n");
}
	
void assertMsg(int time, char *ele, int expectedValue, int realValue, int addr) {
	int en;

	if (GEN_ASSERTS) { 
		genAssertMsg(time, ele, realValue,addr);
		return;  // only generate, not checking
	}	

	// printf("Assert failed. Time: %d; Element: %s; ", time, ele);
	ComputerSystem_DebugMessage(88,ERROR, time, ele);
	en=elementNumber(ele);
	
	if ((en==RMEM_OP) || (en==AMEM_OP) || (en==IR_OP) || (en==MBR_OP)) 
	  	// printf("Expected: '%s'; Real: '%s'", expectedValue, realValue);
	  	ComputerSystem_DebugMessage(89,ERROR, InstructionNames[expectedValue], InstructionNames[realValue]);
	else
		// printf("Expected: %d; Real: %d", expectedValue, realValue);
		ComputerSystem_DebugMessage(90,ERROR, expectedValue, realValue);
	
	if ((en==RMEM_OP) || (en==RMEM_O1) || (en==RMEM_O2) || (en==AMEM_OP) || (en==AMEM_O1) || (en==AMEM_O2) || (en==RMEM) || (en==AMEM)) 
		// printf("; Memory address: %d", addr);
		ComputerSystem_DebugMessage(91,ERROR, addr);
	
	// printf("\n");
	ComputerSystem_DebugMessage(100, ERROR, "\n");
}


void Asserts_CheckAsserts(){
	int na;
	int globalCounter=Clock_GetTime();
	
 	// Checking unique time asserts
	while (Asserts_IsThereANewAssert(globalCounter)>0) {
		na=Heap_poll(assertsQueue,QUEUE_ASSERTS,&numOfElementsInAssertsQueue);
		if (asserts[na].time==globalCounter) {
            Asserts_CheckOneAssert(na);
		}
		else {
			ComputerSystem_DebugMessage(93,ERROR,asserts[na].time,asserts[na].element); 
		}
	}

	na=beginOfAllTimeAsserts;

   // Checking asserts for all time
 	while (na<MAX_ASSERTS) {
 		 Asserts_CheckOneAssert(assertsQueue[na++].info);
 	}
}

void Asserts_CheckOneAssert(int na){
	MEMORYCELL data;
	BUSDATACELL busData;
	int op=elementNumber(asserts[na].element);
	int globalCounter=Clock_GetTime();

	// Decode
	int operationCode;
	int operand1;
	int operand2;
	int memCell;
	int valueInPCB;

	switch (op) {
		case RMEM_OP: 
				busData.cell=mainMemory[MMU_GetBase()+asserts[na].address];
				operationCode=Processor_DecodeOperationCode(busData);
				if ((operationCode!=asserts[na].value) || GEN_ASSERTS)
					assertMsg(globalCounter,elements[RMEM_OP],asserts[na].value,operationCode,asserts[na].address);
				break;
		case RMEM_O1:  	
				busData.cell=mainMemory[MMU_GetBase()+asserts[na].address];
				operand1=Processor_DecodeOperand1(busData);
				if ((operand1!=asserts[na].value) || GEN_ASSERTS)
					assertMsg(globalCounter,elements[RMEM_O1],asserts[na].value,operand1,asserts[na].address);
				break;		
		case RMEM_O2:
				busData.cell=mainMemory[MMU_GetBase()+asserts[na].address];
				operand2=Processor_DecodeOperand2(busData);
				if ((operand2!=asserts[na].value) || GEN_ASSERTS)
					assertMsg(globalCounter,elements[RMEM_O2],asserts[na].value,operand2,asserts[na].address);
			  	break;
		case AMEM_OP: 				       
				busData.cell=mainMemory[MMU_GetBase()+asserts[na].address];
				operationCode=Processor_DecodeOperationCode(busData);
				if ((operationCode!=asserts[na].value) || GEN_ASSERTS)																																			
					assertMsg(globalCounter,elements[AMEM_OP],asserts[na].value,operationCode,asserts[na].address);
		case AMEM_O1:  
				busData.cell=mainMemory[asserts[na].address];
				operand1=Processor_DecodeOperand1(busData);
				if ((operand1!=asserts[na].value)|| GEN_ASSERTS)
					assertMsg(globalCounter,elements[AMEM_O1],asserts[na].value,operand1,asserts[na].address);
			 	break;
		case AMEM_O2:
				busData.cell=mainMemory[asserts[na].address];
				operand2=Processor_DecodeOperand2(busData);
				if ((operand2!=asserts[na].value)|| GEN_ASSERTS)
					assertMsg(globalCounter,elements[AMEM_O2],asserts[na].value,operand2,asserts[na].address);
			 	break;	 		 	
		case PC:  
				if ((registerPC_CPU!=asserts[na].value)|| GEN_ASSERTS)
					assertMsg(globalCounter,elements[PC],asserts[na].value,registerPC_CPU,0);
				break;
		case ACC:  
				if ((registerAccumulator_CPU!=asserts[na].value)|| GEN_ASSERTS)
					assertMsg(globalCounter,elements[ACC],asserts[na].value,registerAccumulator_CPU,0);
				break;
		case IR_OP: 
				operationCode=Processor_DecodeOperationCode(registerIR_CPU);
				if ((operationCode!=asserts[na].value)|| GEN_ASSERTS)
					assertMsg(globalCounter,elements[IR_OP],asserts[na].value,operationCode,0);
				break;
		case IR_O1:
				operand1=Processor_DecodeOperand1(registerIR_CPU);
				if ((operand1!= asserts[na].value)|| GEN_ASSERTS)
					assertMsg(globalCounter,elements[IR_O1],asserts[na].value,operand1,0);
				break;
		case IR_O2:
				operand2=Processor_DecodeOperand2(registerIR_CPU);
				if ((operand2!=asserts[na].value)|| GEN_ASSERTS)
					assertMsg(globalCounter,elements[IR_O2],asserts[na].value,operand2,0);			
				break;
		case PSW:
				if ((registerPSW_CPU!=asserts[na].value)|| GEN_ASSERTS)
					assertMsg(globalCounter,elements[PSW],asserts[na].value,registerPSW_CPU,0);
				break;
		case MAR: 
				if ((registerMAR_CPU!=asserts[na].value)|| GEN_ASSERTS)
					assertMsg(globalCounter,elements[MAR],asserts[na].value,registerMAR_CPU,0);
				break;
		case MBR_OP:
				operationCode=Processor_DecodeOperationCode(registerMBR_CPU);
				if ((operationCode!=asserts[na].value)|| GEN_ASSERTS)
					assertMsg(globalCounter,elements[MBR_OP],asserts[na].value,operationCode,0);
				break;
		case MBR_O1:
				operand1=Processor_DecodeOperand1(registerMBR_CPU);
				if ((operand1!= asserts[na].value)|| GEN_ASSERTS)
					assertMsg(globalCounter,elements[MBR_O1],asserts[na].value,operand1,0);
				break;
		case MBR_O2:
				operand2=Processor_DecodeOperand2(registerMBR_CPU);
				if ((operand2!=asserts[na].value) || GEN_ASSERTS)
					assertMsg(globalCounter,elements[MBR_O2],asserts[na].value,operand2,0);	
				break;
		case MMU_BS:
				if ((MMU_GetBase()!=asserts[na].value) || GEN_ASSERTS)
					assertMsg(globalCounter,elements[MMU_BS],asserts[na].value,MMU_GetBase(),0);
				break;
		case MMU_LM:
				if ((MMU_GetLimit()!=asserts[na].value) || GEN_ASSERTS)
					assertMsg(globalCounter,elements[MMU_LM],asserts[na].value,MMU_GetLimit(),0);
				break;
		case MMU_MAR:
				if ((MMU_GetMAR()!=asserts[na].value) || GEN_ASSERTS)
					assertMsg(globalCounter,elements[MMU_MAR],asserts[na].value,MMU_GetMAR(),0);
				break;
		case MMEM_MAR:
				if ((MainMemory_GetMAR()!=asserts[na].value) || GEN_ASSERTS)
					assertMsg(globalCounter,elements[MMEM_MAR],asserts[na].value,MainMemory_GetMAR(),0);
				break;
		case MMBR_OP:
				MainMemory_GetMBR(&data);
				busData.cell=data;
				operationCode=Processor_DecodeOperationCode(busData);
				if ((operationCode!=asserts[na].value) || GEN_ASSERTS)
					assertMsg(globalCounter,elements[MMBR_OP],asserts[na].value,operationCode,0);
				break;
		case MMBR_O1:
				MainMemory_GetMBR(&data);
				busData.cell=data;
				operand1=Processor_DecodeOperand1(busData);
				if ((operand1!=asserts[na].value) || GEN_ASSERTS)
					assertMsg(globalCounter,elements[MMBR_O1],asserts[na].value,operand1,0);
				break;
		case MMBR_O2:
				MainMemory_GetMBR(&data);
				busData.cell=data;
				operand2=Processor_DecodeOperand2(busData);
				if ((operand2!=asserts[na].value) || GEN_ASSERTS)
					assertMsg(globalCounter,elements[MMBR_O2],asserts[na].value,operand2,0);
				break;
		case XPID:
				if ((executingProcessID!=asserts[na].value) || GEN_ASSERTS)
					assertMsg(globalCounter,elements[XPID],asserts[na].value,executingProcessID,0);
				break;
		case RMEM:
				memCell=mainMemory[MMU_GetBase()+asserts[na].address];
				if (memCell !=asserts[na].value || GEN_ASSERTS)
					assertMsg(globalCounter,elements[RMEM],asserts[na].value,memCell,asserts[na].address);
			  	break;
		case AMEM: 				       
				memCell=mainMemory[asserts[na].address];
				if (memCell!=asserts[na].value || GEN_ASSERTS)
					assertMsg(globalCounter,elements[AMEM],asserts[na].value,memCell,asserts[na].address);
			 	break;
		case MBR:
			if ((registerMBR_CPU.cell != asserts[na].value)|| GEN_ASSERTS)
					assertMsg(globalCounter,elements[MBR],asserts[na].value,registerMBR_CPU.cell,0);
				break;
		case MMBR:
				MainMemory_GetMBR(&data);
				busData.cell=data;
				if ((busData.cell != asserts[na].value)|| GEN_ASSERTS)
					assertMsg(globalCounter,elements[MMBR],asserts[na].value,busData.cell,0);
				break;
		case PCB_ST:
				valueInPCB=processTable[asserts[na].address].state;
				if ((valueInPCB != asserts[na].value) || GEN_ASSERTS)
					assertMsg(globalCounter,elements[PCB_ST],asserts[na].value,valueInPCB,0);			
				break;
		case PCB_PC:
				valueInPCB=processTable[asserts[na].address].copyOfPCRegister;
				if ((valueInPCB != asserts[na].value) || GEN_ASSERTS)
					assertMsg(globalCounter,elements[PCB_PC],asserts[na].value,valueInPCB,0);			
				break;
		case PCB_PR:
				valueInPCB=processTable[asserts[na].address].priority;
				if ((valueInPCB != asserts[na].value) || GEN_ASSERTS)
					assertMsg(globalCounter,elements[PCB_PR],asserts[na].value,valueInPCB,0);			
				break;
		default: ;

	}
}

// This function returns:
// 		-1 if no asserts in assertsQueue
//		1 if any asserts is now
//		0 else
// considered by CheckAsserts to compare at the current time
int Asserts_IsThereANewAssert(int currentTime) {
#ifdef QUEUE_ASSERTS
		int indexInAsserts = Heap_getFirst(assertsQueue,numOfElementsInAssertsQueue);

		if (indexInAsserts < 0)
		  return -1;  // No new asserts to check
		
		if (asserts[indexInAsserts].time <= currentTime)
		  return 1;  //  There'is new assert
#endif		 
		return 0;  //  No assert in current time
}

void Asserts_TerminateAssertions(){
	if (numOfElementsInAssertsQueue)
		// printf("Warning, numOfElementsInAssertsQueue unchecked asserts in Asserts queue !!! );
		ComputerSystem_DebugMessage(92,ERROR,numOfElementsInAssertsQueue);
};