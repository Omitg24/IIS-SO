#ifndef MAINMEMORY_H
#define MAINMEMORY_H

// A memory cell is capable of storing a MEMORYCELL TYPE
typedef int MEMORYCELL;

// Function prototypes
void MainMemory_SetMAR(int);
void MainMemory_GetMBR(MEMORYCELL *);
void MainMemory_SetMBR(MEMORYCELL *);
int MainMemory_GetCTRL();
void MainMemory_SetCTRL(int);

#endif