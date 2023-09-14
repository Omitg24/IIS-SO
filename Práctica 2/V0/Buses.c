#include "Buses.h"
#include "Processor.h"
#include "MainMemory.h"
#include <string.h>
#include <stdlib.h>

//  Function that simulates the delivery of an address by means of the address bus
//  from a hardware component register to another hardware component register
int Buses_write_AddressBus_From_To(int fromRegister, int toRegister) {
  int data;
  switch (fromRegister) {
	case CPU:
	  data=Processor_GetMAR();
	  break;
	default:
	  return Bus_FAIL;
  }
  
  switch (toRegister) {
	case MAINMEMORY:
	  MainMemory_SetMAR(data);
	  break;
	default:
	 return Bus_FAIL;
  }
  return Bus_SUCCESS;
}


//  Function that simulates the delivery of memory word by means of the data bus
//  from a hardware component register to another hardware component register
int Buses_write_DataBus_From_To(int fromRegister, int toRegister) {
	BUSDATACELL *data = (BUSDATACELL *) malloc(sizeof(BUSDATACELL));
	MEMORYCELL memdata;
	switch (fromRegister) {
		case MAINMEMORY:
			MainMemory_GetMBR(&memdata);
			(*data).cell=memdata;
			break;
		case CPU:
			Processor_GetMBR(data);
			break;
		default:
			free(data);
			return Bus_FAIL;
  	}

	switch (toRegister) {
		case MAINMEMORY:
			memdata = (*data).cell;
			MainMemory_SetMBR(&memdata);
			break;
		case CPU:
			Processor_SetMBR(data);
			break;
		default:
  			free(data);
			return Bus_FAIL;
  	}
  
	free(data);
	return Bus_SUCCESS;
}

//  Function that simulates the delivery of control signals of the control bus
//  from a hardware component register to another hardware component register
int Buses_write_ControlBus_From_To(int fromRegister, int toRegister){
	int control;
	switch (fromRegister) {
		case MAINMEMORY:
			control=MainMemory_GetCTRL();
			break;
		case CPU:
			control=Processor_GetCTRL();
			break;
		default:
			return Bus_FAIL;
	}
  
	switch (toRegister) {
		case MAINMEMORY:
			MainMemory_SetCTRL(control);
			break;
		case CPU:
			Processor_SetCTRL(control);
			break;
		default:
	 		return Bus_FAIL;
	}
	return Bus_SUCCESS;

}
