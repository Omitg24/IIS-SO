#include "MainMemory.h"
#include "Buses.h"
#include "ComputerSystem.h"
#include <string.h>

// Main memory can be simulated by a memory cell array
MEMORYCELL mainMemory[MAINMEMORYSIZE];

// Main memory has a MAR register whose value identifies where
// the next read/write operation will take place 
int registerMAR_MainMemory;

// Main memory has a CTRL (control) register whose value indicates the operation to do
int registerCTRL_MainMemory;

// It also has a register that plays the rol of a buffer for the mentioned operations
MEMORYCELL registerMBR_MainMemory;

// Setter for registerMAR_MainMemory
void MainMemory_SetMAR(int addr) {
  registerMAR_MainMemory=addr;
}

// pseudo-getter for the registerMBR_MainMemory
void MainMemory_GetMBR(MEMORYCELL *toRegister) {
  memcpy((void*) toRegister, (void *) (&registerMBR_MainMemory), sizeof(MEMORYCELL));
}

// pseudo-setter for the registerMBR_MainMemory
void MainMemory_SetMBR(MEMORYCELL *fromRegister) {
  memcpy((void*) (&registerMBR_MainMemory), (void *) fromRegister, sizeof(MEMORYCELL));
}

// Getter for the registerCTRL_MainMemory
int MainMemory_GetCTRL() {  
  return registerCTRL_MainMemory;
}

// Setter for registerCTRL_MainMemory
void MainMemory_SetCTRL(int ctrl) {
	registerCTRL_MainMemory=ctrl&0x3;
	switch (registerCTRL_MainMemory) {
  		case CTRLREAD:
  	 		memcpy((void *) (&registerMBR_MainMemory), (void *) (&mainMemory[registerMAR_MainMemory]), sizeof(MEMORYCELL));
  			Buses_write_DataBus_From_To(MAINMEMORY, CPU);
  			break;
  		case CTRLWRITE:
        memcpy((void *) (&mainMemory[registerMAR_MainMemory]), (void *) (&registerMBR_MainMemory), sizeof(MEMORYCELL));
    		break;
  		default:
  			registerCTRL_MainMemory |= CTRL_FAIL;
  			Buses_write_ControlBus_From_To(MAINMEMORY,CPU);
  			return;
  			break;
  	}
  	registerCTRL_MainMemory |= CTRL_SUCCESS;
  	Buses_write_ControlBus_From_To(MAINMEMORY,CPU);
}

