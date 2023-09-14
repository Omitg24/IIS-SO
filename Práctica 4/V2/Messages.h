#ifndef MESSAGES_H
#define MESSAGES_H

#define NUMBEROFMSGS 100
#define MSGMAXIMUMLENGTH 132

#define TEACHER_MESSAGES_FILE "messagesTCH.txt"

typedef struct {
  int number;
  char format[MSGMAXIMUMLENGTH];
} DEBUG_MESSAGES;

extern DEBUG_MESSAGES DebugMessages[NUMBEROFMSGS];

int Messages_Get_Pos(int number);
int Messages_Load_Messages(int, char *);

#endif

