#
# Makefile for the Linux Simulator
#
########################################################

PROGRAM = 	Simulator

# Compilation Details
SHELL = /bin/sh
CC = cc
STDCFLAGS = -g -c -Wall -O0 
INCLUDES =
LIBRERIAS = 
WRAP = -Wl,-wrap,OperatingSystem_InterruptLogic,-wrap,Processor_FetchInstruction,-wrap,Processor_InstructionCycleLoop,-wrap,Processor_DecodeAndExecuteInstruction


${PROGRAM}: Simulator.o Asserts.o Buses.o Clock.o ComputerSystem.o ComputerSystemBase.o Heap.o MainMemory.o Messages.o MMU.o OperatingSystem.o OperatingSystemBase.o Processor.o ProcessorBase.o Wrappers.o
	$(CC) -o ${PROGRAM} Simulator.o Asserts.o Buses.o Clock.o ComputerSystem.o ComputerSystemBase.o Heap.o MainMemory.o Messages.o MMU.o OperatingSystem.o OperatingSystemBase.o Processor.o ProcessorBase.o Wrappers.o $(LIBRERIAS) $(WRAP)

Simulator.o: Simulator.c Simulator.h ComputerSystem.h ComputerSystemBase.h Asserts.h
	$(CC) $(STDCFLAGS) $(INCLUDES) Simulator.c

Asserts.o: Asserts.c Asserts.h MainMemory.h Simulator.h Clock.h ComputerSystemBase.h ComputerSystem.h MMU.h Heap.h Processor.h ProcessorBase.h Buses.h Instructions.def OperatingSystem.h
	$(CC) $(STDCFLAGS) $(INCLUDES) Asserts.c

Buses.o: Buses.c Buses.h MMU.h Processor.h MainMemory.h Simulator.h ProcessorBase.h Instructions.def
	$(CC) $(STDCFLAGS) $(INCLUDES) Buses.c

Clock.o: Clock.c Clock.h Processor.h MainMemory.h Simulator.h ProcessorBase.h Buses.h Instructions.def ComputerSystem.h ComputerSystemBase.h
	$(CC) $(STDCFLAGS) $(INCLUDES) Clock.c

ComputerSystem.o: ComputerSystem.c ComputerSystem.h Simulator.h ComputerSystemBase.h OperatingSystem.h Processor.h MainMemory.h ProcessorBase.h Buses.h Instructions.def Messages.h Asserts.h Wrappers.c Wrappers.h
	$(CC) $(STDCFLAGS) $(INCLUDES) ComputerSystem.c

ComputerSystemBase.o: ComputerSystemBase.c ComputerSystem.h Simulator.h ComputerSystemBase.h Processor.h MainMemory.h ProcessorBase.h Buses.h Instructions.def Heap.h OperatingSystemBase.h OperatingSystem.h Messages.h Asserts.h
	$(CC) $(STDCFLAGS) $(INCLUDES) ComputerSystemBase.c

Heap.o: Heap.c Heap.h OperatingSystem.h ComputerSystem.h Simulator.h ComputerSystemBase.h Asserts.h
	$(CC) $(STDCFLAGS) $(INCLUDES) Heap.c

MainMemory.o: MainMemory.c MainMemory.h Simulator.h Processor.h ProcessorBase.h Buses.h Instructions.def
	$(CC) $(STDCFLAGS) $(INCLUDES) MainMemory.c

Messages.o: Messages.c Messages.h ComputerSystem.h Simulator.h ComputerSystemBase.h
	$(CC) $(STDCFLAGS) $(INCLUDES) Messages.c

MMU.o: MMU.c MMU.h Buses.h Processor.h MainMemory.h Simulator.h ProcessorBase.h Instructions.def
	$(CC) $(STDCFLAGS) $(INCLUDES) MMU.c

OperatingSystem.o: OperatingSystem.c OperatingSystem.h ComputerSystem.h Simulator.h ComputerSystemBase.h OperatingSystemBase.h MMU.h Processor.h MainMemory.h ProcessorBase.h Buses.h Instructions.def Heap.h
	$(CC) $(STDCFLAGS) $(INCLUDES) OperatingSystem.c

OperatingSystemBase.o: OperatingSystemBase.c OperatingSystemBase.h ComputerSystem.h Simulator.h ComputerSystemBase.h OperatingSystem.h Processor.h MainMemory.h ProcessorBase.h Buses.h Instructions.def
	$(CC) $(STDCFLAGS) $(INCLUDES) OperatingSystemBase.c

Processor.o: Processor.c Processor.h MainMemory.h Simulator.h Options.def ProcessorBase.h Buses.h Instructions.def OperatingSystem.h ComputerSystem.h ComputerSystemBase.h OperatingSystemBase.h Heap.h Wrappers.c Wrappers.h Clock.h Asserts.h
	$(CC) $(STDCFLAGS) $(INCLUDES) Processor.c

ProcessorBase.o: ProcessorBase.c Processor.h MainMemory.h Simulator.h Options.def ProcessorBase.h Buses.h Instructions.def Clock.h Asserts.h
	$(CC) $(STDCFLAGS) $(INCLUDES) ProcessorBase.c

Wrappers.o: Wrappers.c Wrappers.h Clock.h Asserts.h
	$(CC) $(STDCFLAGS) $(INCLUDES) Wrappers.c

clean:
	rm -f $(PROGRAM) *.o *~ *.d core
