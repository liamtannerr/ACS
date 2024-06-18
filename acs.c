#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "queue.h"

#define BLUE "\x1b[34m"
#define RESET "\x1b[0m"
#define RED  "\x1b[31m"
#define GREEN "\x1b[32m"


Queue* business_queue;
Queue* economy_queue;
pthread_mutex_t business_mutex;
pthread_mutex_t economy_mutex;
int num_customers;


int compare_arrivals(const void* a, const void* b){
	customer* customer_a = (customer*)a;
	customer* customer_b = (customer*)b;
	return (customer_a->arrival_time - customer_b->arrival_time);
}

void handleArrivals(void* arg){
	customer* customers = (customer*) arg;
	int cur_arrival;
	int prev_arrival = 0;
	int dif;
	for (int i = 0; i < num_customers; i++){
		customer cust = customers [i];
		cur_arrival = cust.arrival_time;
		if(cust.class == 1){
			pthread_mutex_lock(&business_mutex);
			enqueue(business_queue, cust);
			pthread_mutex_unlock(&business_mutex);
		} else {
			pthread_mutex_lock(&economy_mutex);
			enqueue(economy_queue, cust);
			pthread_mutex_unlock(&economy_mutex);
		}
		dif = cur_arrival - prev_arrival;
		usleep(100000*dif);
		prev_arrival = cur_arrival;

	}
}

void* serveCustomer(void* arg) {
	int clerk_id = *(int*)arg;
	struct timespec start_time, end_time;
	while(1){
		customer* cust = NULL;
		pthread_mutex_lock(&business_mutex);
		if(!isEmpty(business_queue)){
			cust = dequeue(business_queue);
		} 
		pthread_mutex_unlock(&business_mutex);
		if(cust == NULL){
			pthread_mutex_lock(&economy_mutex);
			if(!isEmpty(economy_queue)){
				cust = dequeue(economy_queue);
			} 
			pthread_mutex_unlock(&economy_mutex);
		}

		//Serve the dequeued customer
		if(cust != NULL){
			printf("Clerk %d starts serving customer %d (Class: %d, Arrival Time: %d, Service Time: %d)\n",
                   clerk_id, cust->id, cust->class, cust->arrival_time, cust->service_time);
            usleep(cust->service_time * 100000);
		}
	}
}





int main(){
	
	char buf [256];
	
	FILE* customers_file = fopen("customers.txt", "r");
	if (customers_file == NULL){
		printf(RED "Error: Could not open customers file \n" RESET);
		return 0;
	}

	pthread_t clerks [5];
	int clerkIds [5];

	num_customers = atoi(fgets(buf, sizeof(buf), (customers_file)));
	int customer_id;
	int customer_class;
	int customer_arrival_time;
	int customer_service_time;

	customer* customers = malloc(num_customers * sizeof(customer));
	char line [256];
	int num_business = 0;
	int num_economy = 0;
	
	for(int i = 0; i < num_customers; i++){
		if(fgets(line, sizeof(line), customers_file) != NULL){
			sscanf(line, "%d:%d,%d,%d", &customer_id, &customer_class, &customer_arrival_time, &customer_service_time);
			if(customer_class == 1){
				customers[num_business].id = customer_id;
				customers[num_business].class = customer_class;
				customers[num_business].arrival_time = customer_arrival_time;
				customers[num_business].service_time = customer_service_time;				
			}
			if(customer_class == 1){
				num_business++;				
			} else {
				num_economy++;
			}			

		}
	}

	fclose(customers_file);
	qsort(customers, num_business, sizeof(customer), compare_arrivals);

	business_queue = createQueue();
	economy_queue = createQueue();

	pthread_t queueHandler;
	pthread_create(&queueHandler, NULL, handleArrivals, &customers);


	pthread_mutex_init(&business_mutex, NULL);
	pthread_mutex_init(&business_mutex, NULL);

	for(int i = 0; i < 5; i++){
		clerkIds[i] = i + 1;
		pthread_create(&clerks[i], NULL, serveCustomer, &clerkIds[i]);
	}

	pthread_join(queueHandler, NULL);

	for(int i = 0; i < 5; i++){
		pthread_join(clerks[i], NULL);
	}


	
	free(customers);
    return 0;
}





