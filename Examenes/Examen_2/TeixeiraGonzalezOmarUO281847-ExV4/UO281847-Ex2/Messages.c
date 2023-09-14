#include <stdio.h>
#include <string.h>
#include "Messages.h"
#include "ComputerSystem.h"

int Messages_Set(int , char * );

DEBUG_MESSAGES DebugMessages[NUMBEROFMSGS] = {[0 ... NUMBEROFMSGS-1] = {-1,""}};

int Messages_Load_Messages(int numberOfmessages, char * nameFileMessage) {

	char lineRead[MSGMAXIMUMLENGTH];
	FILE *mf;
	char *number, *text;
	int msgNumber;
	int lineNumber=0;;
	int rc;
	int initialMessage=numberOfmessages;

	mf=fopen(nameFileMessage, "r");
	if (mf==NULL) {
	   // printf("Verbose messages unavailable\n");
	   ComputerSystem_DebugMessage(62,POWERON,nameFileMessage);
	   return -1;
	  }
	   
	while (fgets(lineRead, MSGMAXIMUMLENGTH, mf) != NULL) {
		lineNumber++;
		number=strtok(lineRead,",");
 		if ((number!=NULL) && (number[0]!='/') && (number[0]!='\n') && (number[0]!='\r')) {
	  		rc=sscanf(number,"%d",&msgNumber);
	    	if (rc==0){
					// printf("Illegal Message Number in line %d of file %s\n",lineNumber,nameFileMessage);
					ComputerSystem_DebugMessage(60,POWERON,lineNumber,nameFileMessage);
					continue;
				}
		
				text=strtok(NULL,"\n");
	  		if (text==NULL){
					// printf("Illegal Message Format in line %d of file %s\n",lineNumber,nameFileMessage);
					ComputerSystem_DebugMessage(61,POWERON,lineNumber,nameFileMessage);
					continue;
				}
			rc=	Messages_Set(msgNumber,text);
			switch (rc) {
				case -1:
					ComputerSystem_DebugMessage(65,POWERON);
					break;
				case -2:
					ComputerSystem_DebugMessage(66,POWERON,msgNumber);
					break;
				default:
					numberOfmessages++;
				}
		}
	}
  fclose(mf);

  // Remove when finish debug.... 
  // for (rc=0;rc<NUMBEROFMSGS;rc++)
  // 	printf("Position: %d, Message number: %d, Message format: %s\n",rc,DebugMessages[rc].number,DebugMessages[rc].format); 

  // printf("%d Messages Loaded\n",nm);
  ComputerSystem_DebugMessage(63,POWERON,(numberOfmessages-initialMessage), nameFileMessage);

  return numberOfmessages;
}

int Messages_Get_Pos(int number) {
	int pos  = number % NUMBEROFMSGS;
	int i;
	for (i=0; (DebugMessages[pos].number !=-1) && (DebugMessages[pos].number !=number) && (i < NUMBEROFMSGS); i++) 
		pos=(pos+1)%NUMBEROFMSGS;
 	if (DebugMessages[pos].number==number)
 		return pos;

	return -1;
}

int Messages_Set(int msgNumber, char * text) {
	int posBase = msgNumber % NUMBEROFMSGS;
	int i, position=posBase;

	for (i=0;i<NUMBEROFMSGS && DebugMessages[position].number!=-1 && DebugMessages[position].number!=msgNumber;i++) 
		position=(posBase+i)%NUMBEROFMSGS; 
	if (DebugMessages[position].number==-1) {
		strcpy(DebugMessages[position].format,text);
		DebugMessages[position].number=msgNumber;
		return position;
	}
	else 
		if (DebugMessages[position].number==msgNumber)
			return -2;
	return -1;
}
