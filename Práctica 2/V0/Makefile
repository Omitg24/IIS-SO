#
# Makefile for the Linux simulator
#
########################################################

PROGRAM = 	Simulator

# Compilation Details
SHELL = /bin/sh
CC = cc
STDCFLAGS = -g -c -Wall
INCLUDES = 
LIBRARIES = 

${PROGRAM}: Simulator.o ComputerSystem.o MainMemory.o OperatingSystem.o Processor.o Buses.o Messages.o
	$(CC) -o ${PROGRAM} Simulator.o ComputerSystem.o MainMemory.o OperatingSystem.o Processor.o Buses.o Messages.o $(LIBRARIES)

Simulator.o: Simulator.c Simulator.h
	$(CC) $(STDCFLAGS) $(INCLUDES) Simulator.c

Messages.o: Messages.c Messages.h
	$(CC) $(STDCFLAGS) $(INCLUDES) Messages.c

ComputerSystem.o: ComputerSystem.c ComputerSystem.h
	$(CC) $(STDCFLAGS) $(INCLUDES) ComputerSystem.c

MainMemory.o: MainMemory.c MainMemory.h
	$(CC) $(STDCFLAGS) $(INCLUDES) MainMemory.c

OperatingSystem.o: OperatingSystem.c OperatingSystem.h
	$(CC) $(STDCFLAGS) $(INCLUDES) OperatingSystem.c

Processor.o: Processor.c Processor.h
	$(CC) $(STDCFLAGS) $(INCLUDES) Processor.c


Buses.o: Buses.c Buses.h
	$(CC) $(STDCFLAGS) $(INCLUDES) Buses.c

clean:
	rm -f $(PROGRAM) *.o *~ core
