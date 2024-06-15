#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "queue.h"

#define BLUE "\x1b[34m"
#define RESET "\x1b[0m"
#define RED  "\x1b[31m"
#define GREEN "\x1b[32m"

float avg_wait = 0.0;
float avg_ec_wait = 0.0;
float avg_bs_wait = 0.0;


int compare_arrivals(const void* a, const void* b){
	customer* customer_a = (customer*)a;
	customer* customer_b = (customer*)b;
	return (customer_a->arrival_time - customer_b->arrival_time);
}

int main(){
	
	char buf [256];
	
	FILE* customers_file = fopen("customers.txt", "r");
	if (customers_file == NULL){
		printf(RED "Error: Could not open customers file \n" RESET);
		return 0;
	}

	int num_customers = atoi(fgets(buf, sizeof(buf), (customers_file)));
	int customer_id;
	int customer_class;
	int customer_arrival_time;
	int customer_service_time;

	customer* business_customers = malloc(num_customers * sizeof(customer));
	customer* economy_customers = malloc(num_customers * sizeof(customer));
	char line [256];
	int num_business = 0;
	int num_economy = 0;
	
	for(int i = 0; i < num_customers; i++){
		if(fgets(line, sizeof(line), customers_file) != NULL){
			sscanf(line, "%d:%d,%d,%d", &customer_id, &customer_class, &customer_arrival_time, &customer_service_time);
			if(customer_class == 1){
				business_customers[num_business].id = customer_id;
				business_customers[num_business].class = customer_class;
				business_customers[num_business].arrival_time = customer_arrival_time;
				business_customers[num_business].service_time = customer_service_time;
				num_business++;				
			} else {
				economy_customers[num_economy].id = customer_id;
				economy_customers[num_economy].class = customer_class;
				economy_customers[num_economy].arrival_time = customer_arrival_time;
				economy_customers[num_economy].service_time = customer_service_time;
				num_economy++;
			}

		}
	}

	qsort(business_customers, num_business, sizeof(customer), compare_arrivals);
	qsort(economy_customers, num_economy, sizeof(customer), compare_arrivals);

	    printf(BLUE "Business Customers:\n" RESET);
    for (int i = 0; i < num_business; i++) {
        printf("Customer ID: %d, Class: %d, Arrival Time: %d, Service Time: %d\n",
            business_customers[i].id, business_customers[i].class, business_customers[i].arrival_time, business_customers[i].service_time);
    }

    printf(GREEN "Economy Customers:\n" RESET);
    for (int i = 0; i < num_economy; i++) {
        printf("Customer ID: %d, Class: %d, Arrival Time: %d, Service Time: %d\n",
            economy_customers[i].id, economy_customers[i].class, economy_customers[i].arrival_time, economy_customers[i].service_time);
    }

	fclose(customers_file);
	free(business_customers);
	free(economy_customers);
    return 0;
}



