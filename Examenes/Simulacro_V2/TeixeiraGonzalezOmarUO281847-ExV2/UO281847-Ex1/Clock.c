#include "Clock.h"
#include "Processor.h"
#include "ComputerSystemBase.h"

int tics=0;

void Clock_Update() {
	tics++;
	//V2-EX2d-BEGIN
    if (tics%intervalBetweenInterrupts == 0) {
		Processor_RaiseInterrupt(CLOCKINT_BIT);
	}
	//V2-EX2d-END
}


int Clock_GetTime() {

	return tics;
}
