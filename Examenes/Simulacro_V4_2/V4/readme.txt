Omar Teixeira González, UO281847:

Modificaciones:
    En messagesSTD.txt:
        150,Process [@G%d - %s@@] new MMU limit register value is [@G%d@@]\n
        151,ERROR: Process [@G%d - %s@@] with modified size [%d] does not fit in its own partition [%d] but partition [%d] is available\n
        152,ERROR: Process [@G%d - %s@@] with modified size [%d] does not fit in its own partition [%d]\n
        153,ERROR: Process [@G%d - %s@@] size modification request is invalid\n

    En OperatingSystem.h:
        enum SystemCallIdentifiers { SYSCALL_END=3, SYSCALL_YIELD=4, SYSCALL_PRINTEXECPID=5, SYSCALL_SLEEP = 7, SYSCALL_CHANGESIZE = 11};
    
    En OperatingSystem.c:
        // System call management routine
        void OperatingSystem_HandleSystemCall() {  

            int systemCallID;
            int previousProcessID;
            int newProcessID;
            //Simulacro-V4
            int accumulator;
            //Simulacro-V4

            // Register A contains the identifier of the issued system call
            systemCallID=Processor_GetRegisterA();
            
            switch (systemCallID) {
                case SYSCALL_PRINTEXECPID:
                    // Show message: "Process [executingProcessID] has the processor assigned\n"
                    //V2-EX1-BEGIN
                    OperatingSystem_ShowTime(SYSPROC);
                    //V2-EX1-END
                    ComputerSystem_DebugMessage(72,SYSPROC,executingProcessID,programList[processTable[executingProcessID].programListIndex]->executableName);
                    break;

                //V1-EX12-BEGIN
                case SYSCALL_YIELD:
                    if(numberOfReadyToRunProcesses[processTable[executingProcessID].queueID] > 0 && processTable[readyToRunQueue[processTable[executingProcessID].queueID][0].info].priority == processTable[executingProcessID].priority) {
                        previousProcessID = executingProcessID;
                        newProcessID = readyToRunQueue[processTable[executingProcessID].queueID][0].info;				

                        //V2-EX1-BEGIN
                        OperatingSystem_ShowTime(SHORTTERMSCHEDULE);
                        //V2-EX1-END
                        ComputerSystem_DebugMessage(115, SHORTTERMSCHEDULE, previousProcessID, programList[processTable[previousProcessID].programListIndex]->executableName, newProcessID, programList[processTable[newProcessID].programListIndex]->executableName);

                        OperatingSystem_PreemptRunningProcess();
                        OperatingSystem_Dispatch(OperatingSystem_ShortTermScheduler());
                        //V2-EX7a-BEGIN
                        OperatingSystem_PrintStatus();
                        //V2-EX7a-END
                    }
                    break;
                //V1-EX12-END

                case SYSCALL_END:
                    // Show message: "Process [executingProcessID] has requested to terminate\n"
                    //V2-EX1-BEGIN
                    OperatingSystem_ShowTime(SYSPROC);
                    //V2-EX1-END
                    ComputerSystem_DebugMessage(73,SYSPROC,executingProcessID,programList[processTable[executingProcessID].programListIndex]->executableName);
                    OperatingSystem_TerminateProcess();
                    //V2-EX7b-BEGIN
                    OperatingSystem_PrintStatus();
                    //V2-EX7b-END
                    break;

                //V2-EX5d-BEGIN
                case SYSCALL_SLEEP:						
                    OperatingSystem_MoveToTheBLOCKEDState(executingProcessID);
                    OperatingSystem_Dispatch(OperatingSystem_ShortTermScheduler());
                    //V2-EX5e-BEGIN
                    OperatingSystem_PrintStatus();
                    //V2-EX5e-END
                    break;
                //V2-EX5d-END

                //Simulacro-V4
                case SYSCALL_CHANGESIZE:
                    accumulator=Processor_GetAccumulator();
                    if (accumulator>0){
                        partitionsTable[processTable[executingProcessID].numberOfPartition].size+=accumulator;
                    } else if (accumulator<0){
                        partitionsTable[processTable[executingProcessID].numberOfPartition].size+=accumulator;
                        if (partitionsTable[processTable[executingProcessID].numberOfPartition].size<=0){
                            partitionsTable[processTable[executingProcessID].numberOfPartition].size=1;
                        }
                    }
                    OperatingSystem_ShowTime(EXAM);
                    ComputerSystem_DebugMessage(150, EXAM, executingProcessID, programList[processTable[executingProcessID].programListIndex]->executableName, partitionsTable[processTable[executingProcessID].numberOfPartition].size);
                    break;
                //Simulacro-V4

                //V4-EX4a-BEGIN
                default:
                    OperatingSystem_ShowTime(INTERRUPT);
                    ComputerSystem_DebugMessage(141, INTERRUPT, executingProcessID, programList[processTable[executingProcessID].programListIndex]->executableName, systemCallID);
                    //V4-EX4a-END
                    //V4-EX4b-BEGIN
                    OperatingSystem_TerminateProcess();
                    //V4-EX4b-END
                    //V4-EX4c-BEGIN
                    OperatingSystem_PrintStatus();
                    //V4-EX4c-END
            }
        }

        // The LTS is responsible of the admission of new processes in the system.
        // Initially, it creates a process from each program specified in the 
        // 			command line and daemons programs
        int OperatingSystem_LongTermScheduler() {
        
            int PID, i,
                numberOfSuccessfullyCreatedProcesses=0;

            //Simulacro-V4
            int processSize, partitionSize, bestFittingPartition;
            //Simulacro-V4
            
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

                        //Simulacro-V4
                        processSize = processTable[PID].processSize;
                        partitionSize = partitionsTable[processTable[PID].numberOfPartition].size;				
                        if (processSize > partitionSize){
                            bestFittingPartition = OperatingSystem_GetBestFittingPartition();
                            if (bestFittingPartition != MEMORYFULL){
                                OperatingSystem_ShowTime(EXAM);
                                ComputerSystem_DebugMessage(151, EXAM, PID, programList[i]->executableName, processSize, partitionsTable[processTable[PID].numberOfPartition].PID, bestFittingPartition);
                            } else if (bestFittingPartition == MEMORYFULL){
                                OperatingSystem_ShowTime(EXAM);
                                ComputerSystem_DebugMessage(152, EXAM, PID, programList[i]->executableName, processSize, partitionsTable[processTable[PID].numberOfPartition].PID);
                            } else {
                                OperatingSystem_ShowTime(EXAM);
                                ComputerSystem_DebugMessage(153, EXAM, PID, programList[i]->executableName);
                            }
                        }
                        //Simulacro-V4
                        break;
                    //V1-EX6b-END
                    //V4-EX6d-BEGIN
                    case MEMORYFULL:
                        OperatingSystem_ShowTime(ERROR);
                        ComputerSystem_DebugMessage(144,ERROR,programList[i]->executableName);
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
    ./Simulator programSimulacroV4-1