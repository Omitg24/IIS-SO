#ifndef OPERATINGSYSTEM_H
#define OPERATINGSYSTEM_H

#include <stdio.h>

#define SUCCESS 1
#define PROGRAMDOESNOTEXIST -1
#define PROGRAMNOTVALID -2

#define LINEMAXIMUMLENGTH 150

// Functions prototypes
int OperatingSystem_LoadProgram(FILE *, int);

#endif
