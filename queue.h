#ifndef QUEUE_H
#define QUEUE_H

#include <sys/types.h>

// Define the structure for the linked list node
typedef struct customer{

	int id;
	int class;
	int arrival_time;
	int service_time;
}customer;

typedef struct Node{
	customer cust;
	struct Node* next;
}Node;

typedef struct Queue{
	Node* front;
	Node* rear;
	int size;
}Queue;

// Function prototypes
Queue* createQueue();
void enqueue(Queue* q, customer c);
customer* dequeue(Queue* q);
int isEmpty(Queue* q);

#endif
