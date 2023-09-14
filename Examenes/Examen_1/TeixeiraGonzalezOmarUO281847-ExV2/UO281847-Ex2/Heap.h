#ifndef HEAP_H
#define HEAP_H

#define QUEUE_WAKEUP 0
#define QUEUE_PRIORITY 1
#define QUEUE_ASSERTS 3

typedef struct  {
	int info;
	unsigned int insertionOrder;
} heapItem;


// Implements the extraction operation (the element with the highest priority).
// Parameters are:
//    heap: the corresponding queue: readyToRun, asserts, UserProgramList or sleepingQueue
//    queueType: if sleeping queue, QUEUE_WAKEUP; if ready to run queue, QUEUE_PRIORITY; if asserts QUEUE_ASSERTS;
//    numElem: number of current elements inside the queue, if successful is decremented by one
// Returns: the item with the highest priority in the queue, if everything went ok
int Heap_poll(heapItem[], int, int*);

// Implements the insertion operation in a binary heap. 
// Parameters are:
//    info: item to be inserted
//    heap: the corresponding queue: readyToRun, asserts,  UserProgramList or sleepingQueue
//    queueType: if sleeping queue, QUEUE_WAKEUP; if ready to run queue, QUEUE_PRIORITY; if asserts QUEUE_ASSERTS; 
//    numElem: number of current elements inside the queue, if successful is increased by one
//    limit: maximum capacity of the queue
// return 0/-1  ok/fail
int Heap_add(int, heapItem[], int , int*, int);

// Auxiliary function to make comparisons
// Parameters are:
// 	Position one
// 	Position two
//    queueType: if sleeping queue, QUEUE_WAKEUP; if ready to run queue, QUEUE_PRIORITY; if asserts QUEUE_ASSERTS; 
int Heap_compare(heapItem, heapItem, int);

// Return top value of heap
// heap: Binary heap to get top value
// numElem: number of elements actually into the queue
// return more priority item, but not extract from heap
int Heap_getFirst(heapItem[], int);

#endif