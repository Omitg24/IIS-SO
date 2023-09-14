#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ComputerSystem.h"
#include "OperatingSystem.h"
#include "Processor.h"
#include "Messages.h"

char defaultDebugLevel[]="A";
// String specified in the command line to tell the simulator which of its
// sections are interesting for the user so it must show debug messages
// related to them
char *debugLevel=defaultDebugLevel;
  
// Only one colour messages. Set to 1 for more colours checking uppercase in debugLevel
int COLOURED = 0 ;

// Powers on of the Computer System. This function is as primitive as the OS
void ComputerSystem_PowerOn(int argc, char *argv[]) {
  int i, nm;

	// Initial values for the processor registers. In the old days, the
	// CS operator had to initialize them in a similar way
	const int initialValueForPCRegister=230;
	const int initialValueForAccumulatorRegister=0;
	const int initialValueForPSWRegister=128;

	// Load debug messages
	nm=Messages_Load_Messages();
	printf("%d Messages Loaded\n",nm);
	
	// To remember the simulator sections to be message-debugged and if messages must be coloured
	// If parameter exists is debugLevel. Default "A"
	if (argc==2) 
		debugLevel = argv[1];
	for (i=0; i< strlen(debugLevel);i++)
		if (isupper(debugLevel[i])){
			COLOURED = 1;
			debugLevel[i]=tolower(debugLevel[i]);
		}
	
	
	FILE *programFile;
	// Initialize processor registers
	Processor_InitializeRegisters(initialValueForPCRegister, initialValueForAccumulatorRegister, initialValueForPSWRegister);
	
	// If PROGRAM_TO_BE_EXECUTED exists, is executed
	programFile= fopen(PROGRAM_TO_BE_EXECUTED, "r");
	
	// Check if programFile exists, if not, poweroff system
	if (programFile==NULL)
		ComputerSystem_PowerOff();
	
	// Load the program in main memory, beginning at the address given by the second argument
	OperatingSystem_LoadProgram(programFile, initialValueForPCRegister);
	
	// Tell the processor to begin its instruction cycle 
	Processor_InstructionCycleLoop();
	
}


// Powers off the CS (the C program ends)
void ComputerSystem_PowerOff() {
	ComputerSystem_DebugMessage(99, SHUTDOWN);
	exit(0);
}


// Function used to show messages with details of the internal working of
// the simulator
// IT IS NOT NECESSARY TO UNDERSTAND ALL THE DETAILS OF THIS FUNCTION
void ComputerSystem_DebugMessage(int msgNo, char section, ...) {

	va_list lp;
	char c, *format;
	int count, youHaveToContinue, colour;
 
	int pos;
	
        pos=Messages_Get_Pos(msgNo);
        if (pos==-1) {
         printf("Debug Message %d not defined\n",msgNo);
         return;
        }
        format=DebugMessages[pos].format;
        	
	va_start(lp, section);
	
	if ((strchr(debugLevel, ALL) != NULL) 		// Print the message because the user specified ALL
	  || section == ERROR  						//  Always print ERROR section
	  || (strchr(debugLevel,section)) != NULL){ //  or the section argument is included in the debugLevel string
		
		for (count = 0, youHaveToContinue = 1, colour=0; youHaveToContinue == 1; count++) {
			//printf("(%c)",format[count]);
			switch (format[count]) {
				case '\\':count++;
				 	 switch (format[count]) {
						case 'n': printf("\n");
							  break;
						case 't': printf("\t");	
							  break;
						default: printf("\%c", format[count]);
					} // switch Control Chars
					break;
				// case '%':			
				case '@':	// Next color char		
		 			count++;
					switch (format[count]) {
			 			case 'R': // Text in red
							if (COLOURED){
							  printf("%c[%d;%dm", 0x1B, 1, 31);
							  if (!colour) colour=1;
							}
						break;
						case 'G': // Text in green
							if (COLOURED){
						  	printf("%c[%d;%dm", 0x1B, 1, 32);
						  	if (!colour) colour=1;
							}
							break;
						case 'Y': // Text in yellow
		  					if (COLOURED){
							  printf("%c[%d;%dm", 0x1B, 1, 33);
							  if (!colour) colour=1;
							}
							break;
						case 'B': // Text in blue
							if (COLOURED){
							  printf("%c[%d;%dm", 0x1B, 1, 34);
						  	if (!colour) colour=1;
							}
							break;
						case 'M': // // Text in magenta
							if (COLOURED){
						  	printf("%c[%d;%dm", 0x1B, 1, 35);
						  	if (!colour) colour=1;
							}
							break;
						case 'C': // Text in cyan
							if (COLOURED){
							  printf("%c[%d;%dm", 0x1B, 1, 36);
							  if (!colour) colour=1;
							}
							break;		
						case 'W': // Text in white
							if (COLOURED){
							  printf("%c[%d;%dm", 0x1B, 1, 37);
							  if (!colour) colour=1;
							}
							break;
						case '@': // Text without color
							if (COLOURED && colour)
							    printf("%c[%dm", 0x1B, 0);
							break;	
					}	// switch colors chars
					break;
				case '%':			
		 			count++;
					switch (format[count]) {
						case 's':
							printf("%s",va_arg(lp, char *));
							// if (colour)
								// printf("%c[%dm", 0x1B, 0);
							break;
						case 'd':
							printf("%d",va_arg(lp, int));
							// if (colour) 
								// printf("%c[%dm", 0x1B, 0);
							break;
						case 'f':
							printf("%f",va_arg(lp, double));
							// if (colour) 
								// printf("%c[%dm", 0x1B, 0);
							break;
						case 'c':
							c = (char) va_arg(lp, int);
							printf("%c", c);
							// if (colour) 
								// printf("%c[%dm", 0x1B, 0);
							break;
						case 'x':
							// c = (char) va_arg(lp, int);
							printf("%08X", va_arg(lp, int));
							// if (colour) 
								// printf("%c[%dm", 0x1B, 0);
							break;
						default:
							youHaveToContinue = 0;
						} // switch format chars
					break;
				default:if (format[count]==0)
						youHaveToContinue=0;
					else
						printf("%c",format[count]);				
			} // switch
		} // for
		va_end(lp);
	} // if
	if (COLOURED && colour)
	    printf("%c[%dm", 0x1B, 0);
} // ComputerSystem_DebugMessage()
