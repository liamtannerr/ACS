#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "queue.h"

//Define Red for error printing
#define RESET "\x1b[0m"
#define RED  "\x1b[31m"

//Declare global variables
Queue* business_queue;
Queue* economy_queue;
pthread_mutex_t business_mutex;
pthread_mutex_t economy_mutex;
pthread_mutex_t processed_mutex;
int num_customers;
int num_customers_processed = 0;
double total_business_wait = 0;
double total_economy_wait = 0;
struct timeval start_time;

//Function to compare customers by arrival used for sorting
int compare_arrivals(const void* a, const void* b){
	customer* customer_a = (customer*)a;
	customer* customer_b = (customer*)b;
	return (customer_a->arrival_time - customer_b->arrival_time);
}

//Function to retrieve the current time relative to the simulation start time
double get_relative_time(){
	struct timeval current_time;
	gettimeofday(&current_time, NULL);
	return (current_time.tv_sec - start_time.tv_sec) + (current_time.tv_usec - start_time.tv_usec) / 1e6;
}

//Function to place customers in their queues
void* handleArrivals(void* arg){
    customer* customers = (customer*)arg;
    //Variables for sleep calculation
    int cur_arrival = customers[0].arrival_time;
    int next_arrival;
    int dif;
    struct timespec ts;

    //Loop through array of all customers sorted by arrival time
    for (int i = 0; i < num_customers; i++) {
        customer cust = customers[i];
        double relative_time = get_relative_time(); // Get the current time
        printf("Customer %d arrives at %.2f seconds\n", cust.id, relative_time);

        //If business class, place in business queue
        if (cust.class == 1) {
            pthread_mutex_lock(&business_mutex); //Mutex to guard the business queue
            enqueue(business_queue, cust);
            relative_time = get_relative_time();
            printf("Customer %d enters the business queue. The length of the business queue is now: %d at %.2f seconds\n", cust.id, business_queue->size, relative_time);
            pthread_mutex_unlock(&business_mutex);

        //Otherwise place in economy queue
        } else {
            pthread_mutex_lock(&economy_mutex); //Mutex to guard the economy queue
            enqueue(economy_queue, cust);
            relative_time = get_relative_time(); 
            printf("Customer %d enters the economy queue. The length of the economy queue is now: %d at %.2f seconds\n", cust.id, economy_queue->size, relative_time);
            pthread_mutex_unlock(&economy_mutex);
        }

        //Calculate the difference between adjacent customer arrival times and sleep for this difference
        if (i + 1 < num_customers) {
            cur_arrival = cust.arrival_time;
            next_arrival = customers[i + 1].arrival_time;
            dif = next_arrival - cur_arrival;
            usleep(100000 * dif);
        }
    }

    return NULL;
}

//Function to act as a clerk; dequeing and serving customers
void* serveCustomer(void* arg) {
    int clerk_id = *(int*)arg;
    double relative_time;
    //While there are still customers to serve, serve them:
    while (num_customers_processed < num_customers) {
        customer* cust = NULL;

        //First, check business queue
        pthread_mutex_lock(&business_mutex);
        if (!isEmpty(business_queue)) {
            cust = dequeue(business_queue);
        }
        pthread_mutex_unlock(&business_mutex);

        //Then check economy queue
        if (cust == NULL) {
            pthread_mutex_lock(&economy_mutex);
            if (!isEmpty(economy_queue)) {
                cust = dequeue(economy_queue);
            }
            pthread_mutex_unlock(&economy_mutex);
        }

        //If a customer has been dequeued, serve them
        if (cust != NULL) {
            relative_time = get_relative_time();
            if (cust->class == 1) {
                printf("Clerk %d STARTS serving customer %d from the business queue at %.2f seconds.\n",
                       clerk_id, cust->id, relative_time);
                       total_business_wait += relative_time - (cust->arrival_time * 0.1);
            } else {
                printf("Clerk %d STARTS serving customer %d from the economy queue at %.2f seconds.\n",
                       clerk_id, cust->id, relative_time);
                       total_economy_wait += relative_time - (cust->arrival_time * 0.1);
            }

            usleep(cust->service_time * 100000); //Sleep to simulate service time

            relative_time = get_relative_time();
            if (cust->class == 1) {
                printf("Clerk %d FINISHES serving customer %d from the business queue at %.2f seconds.\n",
                       clerk_id, cust->id, relative_time);
            } else {
                printf("Clerk %d FINISHES serving customer %d from the economy queue at %.2f seconds.\n",
                       clerk_id, cust->id, relative_time);
            }

            pthread_mutex_lock(&processed_mutex);
            num_customers_processed++; //Increment the number of customers processed
            pthread_mutex_unlock(&processed_mutex);

            free(cust); //Deallocate memory
        }

    }
    return NULL;
}

int main(){
    char buf [256];

    //Open customers file
    FILE* customers_file = fopen("customers.txt", "r");
    if (customers_file == NULL){
        printf(RED "Error: Could not open customers file \n" RESET);
        return 0;
    }

    //Read the first line to retrieve the total number of customers
    num_customers = atoi(fgets(buf, sizeof(buf), customers_file));
    if (num_customers == 0){
        printf(RED "There are no customers to serve\n" RESET);
        return 0;
    }

    int customer_id;
    int customer_class;
    int customer_arrival_time;
    int customer_service_time;

    customer* customers = malloc(num_customers * sizeof(customer)); //Create array for all customers
    char line [256];
    int num_business = 0;
    int num_economy = 0;

    //Read all customers from input file
    for(int i = 0; i < num_customers; i++){
        if(fgets(line, sizeof(line), customers_file) != NULL){
            //Store all customer data
            sscanf(line, "%d:%d,%d,%d", &customer_id, &customer_class, &customer_arrival_time, &customer_service_time);
            customers[i].id = customer_id;
            customers[i].class = customer_class;
            customers[i].arrival_time = customer_arrival_time;
            customers[i].service_time = customer_service_time;               
            if(customer_class == 1){
                num_business++;                
            } else {
                num_economy++;
            }            

        }
    }

    fclose(customers_file); //Close input file
    qsort(customers, num_customers, sizeof(customer), compare_arrivals); //Sort customer array by arrival time

    //Create both queues
    business_queue = createQueue();
    economy_queue = createQueue();

    //Initialize all mutexes
    pthread_mutex_init(&business_mutex, NULL);
    pthread_mutex_init(&economy_mutex, NULL);
    pthread_mutex_init(&processed_mutex, NULL);

    gettimeofday(&start_time, NULL); //Get time of simulation start
    usleep(100000 * customers[0].arrival_time); //Wait for the first customer to arrive


    pthread_t queueHandler;
    pthread_create(&queueHandler, NULL, handleArrivals, customers); //Create thread to place customers in queues

    //Create all 5 clerk threads
    pthread_t clerks [5];
    int clerkIds [5];
    for(int i = 0; i < 5; i++){
        clerkIds[i] = i + 1;
        pthread_create(&clerks[i], NULL, serveCustomer, &clerkIds[i]);
    }

    //Join all threads
    pthread_join(queueHandler, NULL);
    for(int i = 0; i < 5; i++){
        pthread_join(clerks[i], NULL);
    }  

    double total_wait = total_business_wait + total_economy_wait; //For avg wait calculation
    
    if(!num_business == 0){
        printf("The average wait for business customers was %.2f seconds\n", total_business_wait/num_business);
    } else {
        printf("There were no business class customers\n");
    }
    if(!num_economy == 0){
        printf("The average wait for economy customers was %.2f seconds\n", total_economy_wait/num_economy);
    } else {
        printf("There were no economy class customers\n");
    }
    printf("The average wait amongst all customers was %.2f seconds\n", total_wait/num_customers);

    //Destroy all mutexes
    pthread_mutex_destroy(&business_mutex);
    pthread_mutex_destroy(&economy_mutex);
    pthread_mutex_destroy(&processed_mutex);
    free(customers);// Deallocate memory
    return 0;
}