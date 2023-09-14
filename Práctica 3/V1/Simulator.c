#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Simulator.h"
#include "ComputerSystem.h"
#include "OperatingSystemBase.h"
#include "Asserts.h"

// Functions prototypes
int Simulator_GetOption(char *);

extern int initialPID;
extern int endSimulationTime; // For end simulation forced by time
extern char *debugLevel;

char *options[] = {
"NONEXISTING_OPTION",
#define OPTION(name,defValue) #name, // #name cast parameter name to String
#include "Options.def"
#undef OPTION
NULL,
};

char *optionsDefault[]={
"NONEXISTING_OPTION_DEFAULT",
#define OPTION(name,defValue) #defValue, // #name cast parameter name to String
#include "Options.def"
#undef OPTION
NULL,
};

int main(int argc, char *argv[]) {
  
	int paramIndex=1; // argv index
	int i, rc, numPrograms=0, isOption=1;
	char *option, *optionValue;

	// the options must be always before programs
	for (i=paramIndex; i < argc && isOption ;) {
		if (argv[i][0]=='-' && argv[i][1]=='-') {
			option=strtok((char *)&(argv[i][2]),"=");
			optionValue=strtok(NULL," ");
			int optionIndex=Simulator_GetOption(option);
			switch (optionIndex) {
				// case INITIALPID:
				case initialPID_OPT:
					if (optionValue==NULL || sscanf(optionValue,"%d",&initialPID)==0) 
							initialPID=PROCESSTABLEMAXSIZE-1;
					break;
				// case ENDSIMULATIONTIME:
				case endSimulationTime_OPT:
					if (optionValue==NULL || sscanf(optionValue,"%d",&endSimulationTime)==0)
						endSimulationTime=-1;
					break;
				// case NUMASSERTS:
				case numAsserts_OPT:
					if (optionValue==NULL) 
						MAX_ASSERTS=500;
					else {
						rc=sscanf(optionValue,"%d",&MAX_ASSERTS);
						if (rc<=0 || MAX_ASSERTS<=0)
							MAX_ASSERTS=500;
					}
					break;
				// case ASSERTSFILE:
				case assertsFile_OPT:
					if (optionValue==NULL){
						optionValue=(char *) malloc((strlen(optionsDefault[assertsFile_OPT])+1)*sizeof(char));
						strcpy(optionValue,optionsDefault[assertsFile_OPT]);
						// LOAD_ASSERTS_CONF=1;
					}
					strcpy(ASSERTS_FILE,optionValue);
					break;
				// case MESSAGESFILE:
				case messagesSTDFile_OPT:
					if (optionValue==NULL){
						optionValue=(char *) malloc((strlen(optionsDefault[messagesSTDFile_OPT])+1)*sizeof(char));
						strcpy(optionValue,optionsDefault[messagesSTDFile_OPT]);
						// LOAD_ASSERTS_CONF=1;
					}
					strcpy(STUDENT_MESSAGES_FILE,optionValue);
					break;
				// case DEBUGSECTIONS:
				case debugSections_OPT:
					if (optionValue==NULL){
						optionValue=(char *) malloc((strlen(optionsDefault[debugSections_OPT])+1)*sizeof(char));
						strcpy(optionValue,optionsDefault[debugSections_OPT]);
					}
					debugLevel=optionValue;
					break;
				// case DAEMONS_PROGRAMS_FILE:
				case daemonsProgramsFile_OPT:
					if (optionValue==NULL){
						optionValue=(char *) malloc((strlen(optionsDefault[daemonsProgramsFile_OPT])+1)*sizeof(char));
						strcpy(optionValue,optionsDefault[messagesSTDFile_OPT]);
						// LOAD_ASSERTS_CONF=1;
					}
					strcpy(DAEMONS_PROGRAMS_FILE,optionValue);
					break;
				// case GENERATEASSERTS:
				case generateAsserts_OPT:
					GEN_ASSERTS=1; 
					break;
				// case HELP:
				case help_OPT:
					{
						int j;
						printf("Use one or more of these options:\n");
						for (j=1; options[j]!=NULL; j++)
							if (j<generateAsserts_OPT)
								printf("\t%s=ValueOfOption  [%s]\n",options[j], optionsDefault[j]);
							else
								printf("\t%s\n",options[j]);
					}
					break;
				default :
					printf("Invalid option: %s\n", option);
					break;
			}
			i++;
		}
		else isOption=0;
	}

	// Now we counting the programs in command line		
	paramIndex=i;
	for (i=paramIndex;i<argc;){
		rc=strncmp(argv[i],"--",2);
		i++;
		if (rc!=0) {
			numPrograms++;
			if ((i < argc)
			 && (sscanf(argv[i], "%d", &rc) == 1))
				// An arrival time has been read. Increment i
				i++;
		}
		else {
			numPrograms=-1; // Options beetwen programs are not allowed
			break;
		}
	}

	// We now have a multiprogrammed computer system
	// No more than PROGRAMSMAXNUMBER in the command line
	if ((numPrograms<=0) || (numPrograms>PROGRAMSMAXNUMBER)) {
		printf("USE: Simulator [--optionX=optionXValue ...] <program1> [arrivalTime] [<program2> [arrivalTime] .... <program%d [arrivalTime]] \n",PROGRAMSMAXNUMBER);
		if (numPrograms<0)
			printf("Options must be before program names !!!\n");
		else 
			printf("Must have beetwen 1 and %d program names !!!\n", PROGRAMSMAXNUMBER);
		exit(-1);
	}



	// The simulation starts
	ComputerSystem_PowerOn(argc, argv, paramIndex);
	// The simulation ends
	ComputerSystem_PowerOff();
	return 0;
}

// to get the index number of the option
int Simulator_GetOption(char *option){
	int i;
	for (i=0; options[i]!=NULL ; i++)
		if (strcasecmp(options[i],option)==0)
			return i;
	return -1;
}
