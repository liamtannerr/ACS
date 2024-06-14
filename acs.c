#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define BLUE "\x1b[34m"
#define RESET "\x1b[0m"
#define RED  "\x1b[31m"
#define GREEN "\x1b[32m"

float avg_wait = 0.0;
float avg_ec_wait = 0.0;
float avg_bs_wait = 0.0;

typedef struct customer{

	int id;
	int class;
	int arrival_time;
	int service_time;

}customer;

int main(){
	
	char buf [256];
	
	FILE* customers_file = fopen("customers.txt", "r");
	if (customers_file == NULL){
		printf(RED "Error: Could not open customers file \n" RESET);
		return 0;
	}

	int num_customers = atoi(fgets(buf ,2,(customers_file)));

	int customer_id;
	int customer_class;
	int customer_arrival_time;
	int customer_service_time;

	customer* customers = malloc(num_customers * sizeof(customer));

	
	
	while (!feof(customers_file)) {


    
		scanf(line, "", &customer_id, &customer_class, &customer_arrival_time, &customer_service_time);
 	
	        
	}
	
 
	fclose(customers_file);
 
    return 0;
}
