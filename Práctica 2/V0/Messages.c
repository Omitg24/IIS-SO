#include "Messages.h"

#include <stdio.h>
#include <string.h>

#define MSGMAXIMUMLENGTH 132
#define MESSAGES_FILE "messages.txt"
#define NUMBEROFMSGS 64



int Messages_Load_Messages() {
 char lineRead[MSGMAXIMUMLENGTH];
 FILE *mf;
 char *number, *text;
 int msgNumber;
 int lineNumber=0;;
 int numberErrorMessages=0;
 int rc;

 mf=fopen(MESSAGES_FILE, "r");
 if (mf==NULL) {
   printf("Verbose messages unavailable\n");
   return -1;
  }
   
   while (fgets(lineRead, MSGMAXIMUMLENGTH, mf) != NULL) {
	lineNumber++;	
	number=strtok(lineRead,",");
        rc=sscanf(number,"%d",&msgNumber);
        if (rc==0){
 		printf("Illegal Message Number in line %d of file %s\n",lineNumber,MESSAGES_FILE);
		continue;
	}
	
	text=strtok(NULL,"\n");
        if (text==NULL){
 		printf("Illegal Message Format in line %d of file %s\n",lineNumber,MESSAGES_FILE);
		continue;
	}
	strcpy(DebugMessages[numberErrorMessages].format,text);
	DebugMessages[numberErrorMessages++].number=msgNumber;

	
   }
   fclose(mf);
   return numberErrorMessages;
}

int Messages_Get_Pos(int number) {
 int pos;

 for (pos=0; DebugMessages[pos].number !=-1; pos++)
  if (DebugMessages[pos].number==number) return pos;

 return -1;
}
