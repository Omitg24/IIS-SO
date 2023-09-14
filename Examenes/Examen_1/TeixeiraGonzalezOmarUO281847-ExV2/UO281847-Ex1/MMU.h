#ifndef MMU_H
#define MMU_H

// Functions prototypes
void MMU_SetCTRL(int);
int MMU_GetCTRL();
int MMU_GetMAR();
void MMU_SetMAR(int);
void MMU_SetBase(int);
void MMU_SetLimit(int);

// Used for Asserts
int MMU_GetBase();
int MMU_GetLimit();

#endif