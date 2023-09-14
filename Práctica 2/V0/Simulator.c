#include <stdio.h>
#include <stdlib.h>
#include "Simulator.h"
#include "ComputerSystem.h"


int main(int argc, char *argv[]) {

	// Our Computer System is very primitive. It is monoprogrammed
	// so it can only execute one program per simulation session.
	// The program to be executed is specified in the function
	// ComputerSystem_PowerOn()
	
	if (argc > 2) {
		printf("USE: Simulator [sections_to_be_debugged]\n");
		exit(-1);
	}

	// The simulation starts
	ComputerSystem_PowerOn(argc, argv);
	// The simulation ends
	ComputerSystem_PowerOff();
	return 0;
}
