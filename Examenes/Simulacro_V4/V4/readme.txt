Omar Teixeira González, UO281847:

Modificaciones:
    En messagesSTD.txt:
        150,Process creation from program [@G%s@@] has been postponed\n

    En OperatingSystem.c:
        int OperatingSystem_LongTermScheduler() {  
            int PID, i,
                numberOfSuccessfullyCreatedProcesses=0;

            i =0;
            //V3-EX3-BEGIN
            while (OperatingSystem_IsThereANewProgram() == YES) {
                i = Heap_poll(arrivalTimeQueue, QUEUE_ARRIVAL, &numberOfProgramsInArrivalTimeQueue);
            //V3-EX3-END
                PID=OperatingSystem_CreateProcess(i);
                switch (PID) {
                    //V1-EX4b-BEGIN
                    case NOFREEENTRY:
                        //V2-EX1-BEGIN
                        OperatingSystem_ShowTime(ERROR);
                        //V2-EX1-END
                        ComputerSystem_DebugMessage(103, ERROR, programList[i]->executableName);
                        break;
                    //V1-EX4b-END
                    //V1-EX5a-BEGIN
                    case PROGRAMDOESNOTEXIST:
                        //V2-EX1-BEGIN
                        OperatingSystem_ShowTime(ERROR);
                        //V2-EX1-END
                        ComputerSystem_DebugMessage(104, ERROR, programList[i]->executableName, "it does not exist");
                        break;
                    case PROGRAMNOTVALID:
                        //V2-EX1-BEGIN
                        OperatingSystem_ShowTime(ERROR);
                        //V2-EX1-END
                        ComputerSystem_DebugMessage(104, ERROR, programList[i]->executableName, "invalid priority or size");	
                        break;
                    //V1-EX5a-END
                    //V1-EX6b-BEGIN
                    case TOOBIGPROCESS:
                        //V2-EX1-BEGIN
                        OperatingSystem_ShowTime(ERROR);
                        //V2-EX1-END
                        ComputerSystem_DebugMessage(105, ERROR, programList[i]->executableName);	
                        break;
                    //V1-EX6b-END
                    //V4-EX6d-BEGIN
                    case MEMORYFULL:
                        OperatingSystem_ShowTime(ERROR);
                        ComputerSystem_DebugMessage(144,ERROR,programList[i]->executableName);
                        //Simulacro-V4
                        OperatingSystem_ShowTime(SYSPROC);
                        ComputerSystem_DebugMessage(150,SYSPROC,programList[i]->executableName);
                        programList[i]->arrivalTime+=5;				
                        //Simulacro-V4
                        break;
                    //V4-EX6d-END
                default:
                    numberOfSuccessfullyCreatedProcesses++;
                    if (programList[i]->type==USERPROGRAM) 
                        numberOfNotTerminatedUserProcesses++;		
                    // Move process to the ready state
                    OperatingSystem_MoveToTheREADYState(PID);			
                }
            }
            //V2-EX7d-BEGIN
            if (numberOfSuccessfullyCreatedProcesses>0){
                OperatingSystem_PrintStatus();
            }	
            //V2-EX7d-END
            // Return the number of succesfully created processes
            return numberOfSuccessfullyCreatedProcesses;
        }

Probado con la salida:
./Simulator Pr1 10 Pr1 10 Pr2 10 Pr3 9 Pr3 30 Pr3 30