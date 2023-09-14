#ifndef BUSES_H
#define BUSES_H

enum BusConnection { MAINMEMORY, MMU, CPU, INPUTDEVICE, OUTPUTDEVICE };

#define Bus_SUCCESS 1
#define Bus_FAIL -1

#define CTRLREAD 0x1
#define CTRLWRITE 0x2
#define CTRL_SUCCESS 0x10
#define CTRL_FAIL 0x20

typedef struct {
	int cell;
} BUSDATACELL;

// Functions prototypes
int Buses_write_AddressBus_From_To(int, int);
int Buses_write_DataBus_From_To(int, int);
int Buses_write_ControlBus_From_To(int, int);

#endif