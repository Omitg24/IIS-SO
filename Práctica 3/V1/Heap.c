#include <stdlib.h>
#include "Heap.h"
#include "OperatingSystemBase.h"
#include "Asserts.h"

// Internal Functions prototypes
void Heap_swap_Up(int, heapItem[], int);
void Heap_swap_Down(int, heapItem[], int, int);
int Heap_InsertionTime(unsigned int , unsigned int );

int counter=1;

// Insertion of a PID into a binary heap
// info: PID to insert
// heap: Binary heap to insert: user o daemon ready queue, sleeping queue, ...
// queueType: QUEUE_PRIORITY, QUEUE_ARRIVAL, ...
// numElem: number of elements actually into the queue, if successful is increased by one
// limit: max size of the queue
// return 0/-1  ok/fail
int Heap_add(int info, heapItem heap[], int queueType, int *numElem, int limit) {
	if (*numElem >= limit || info<0)
		return -1;
	heap[*numElem].info=info;
	heap[*numElem].insertionOrder=counter++;
	Heap_swap_Up(*numElem, heap, queueType);
	(*numElem)++;
	return 0;
}

// Extract the more priority item
// heap: Binary heap to extract: user o daemon ready queue, sleeping queue, ...
// queueType: QUEUE_PRIORITY, QUEUE_ARRIVAL, ...
// numElem: number of elements actually into the queue, if successful is decremented by one
// return more priority item into the queue
int Heap_poll(heapItem heap[], int queueType, int *numElem) {
	int info = heap[0].info;
	if (*numElem==0)
		return -1; // no elements into priority queue
	else {
		heap[0]=heap[*numElem-1];
		Heap_swap_Down(0, heap, queueType, *numElem);
		(*numElem)--;
	}
	return info;		
}

// Return top value of heap
// heap: Binary heap to get top value
// numElem: number of elements actually into the queue
// return more priority item, but not extract from heap
int Heap_getFirst(heapItem heap[], int numElem) {
	if (numElem>0) 
		return heap[0].info;
	else
		return -1;
}

//  Auxiliary function for implementation of binary heaps
void Heap_swap_Up(int p, heapItem heap[], int queueType) {
	if (p > 0)  { // if not at the top...
		int padre = abs(p - 1) / 2; // integer operation
		if (Heap_compare(heap[p],heap[padre],queueType)>0) { // less than father...
			heapItem aux = heap[padre]; 
			heap[padre] = heap[p];
			heap[p] = aux;
			Heap_swap_Up(padre, heap, queueType);
		} // if not less, don't switch
	}  // at the top...
}

//  Auxiliary function for implementation of binary heaps
void Heap_swap_Down(int p, heapItem heap[], int queueType, int numElem) {
	int izq = 2*p+1;
	int der = 2*p+2;
	heapItem aux = heap[p];
	
	if (der < numElem) //  2 children... 
		if ((Heap_compare(heap[izq],heap[der], queueType)>0) && (Heap_compare(heap[p],heap[izq],queueType)<0)){ // Switch with left-child if right-child greater
			heap[p] = heap[izq];
			heap[izq] = aux;
			Heap_swap_Down(izq, heap, queueType, numElem);
		} else { // right 
			if (Heap_compare(heap[p],heap[der], queueType)<0) { // Switch with right-child
				heap[p] = heap[der];
				heap[der] = aux;
				Heap_swap_Down(der, heap, queueType, numElem);
			}
		}
	else if (izq<numElem) { // only left-child...
		if (Heap_compare(heap[p],heap[izq], queueType)<0){ // Switch with left-child
			heap[p] = heap[izq];
			heap[izq] = aux;
			Heap_swap_Down(izq, heap, queueType, numElem);
		} // Less than left-child, don't switch
	} // leaf-node...
}

// Auxiliary for priority comparations
int Heap_compare_priority(int value1, int value2) {
  return processTable[value2].priority-processTable[value1].priority;
}

// Auxiliary for assert-time comparations
int Heap_compare_assertsTime(int value1, int value2) {
  return asserts[value2].time - asserts[value1].time;
}

// Auxiliary for generic comparations
int Heap_compare(heapItem value1, heapItem value2, int queueType) {
  int primaryKey=0;
  switch (queueType) {
	case QUEUE_PRIORITY:
		primaryKey= Heap_compare_priority(value1.info, value2.info);
		break;
	case QUEUE_ASSERTS:
		primaryKey= Heap_compare_assertsTime(value1.info, value2.info);
		break;
  }
  
  if (primaryKey==0)
	return Heap_InsertionTime(value1.insertionOrder,value2.insertionOrder);
  return primaryKey; // 

}

// Auxiliary for secondaryKey comparations
int Heap_InsertionTime(unsigned int value1, unsigned int value2){
	return value2 - value1;
}
