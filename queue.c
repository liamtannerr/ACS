#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

// Function to create a queue
Queue* createQueue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q -> front = NULL;
    q -> rear = NULL;
    q -> size = 0;
    return q;
}

// Function to add a customer to the queue
void enqueue(Queue* q, customer c) {
    Node* n = (Node*)malloc(sizeof(Node));
    n -> cust = c;
    n -> next = NULL;
    if(isEmpty(q)){
        q -> front = n;
        q -> rear = n;
    } else {
        q -> rear -> next = n;
        q -> rear = n;
    }
    q -> size++;
}

// Function to remove a customer from the queue
customer* dequeue(Queue* q) {

}

// Function to check if the queue is empty
int isEmpty(Queue* q) {
    
}
