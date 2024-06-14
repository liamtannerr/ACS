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


int main(){
	
	
	FILE* customers_file = fopen("customers.txt", "r");
	if (customers_file == NULL){
		printf(RED "Error: Could not open customers file" RESET);
		return 0;
	}
	char num_customers [1];
	fgets(num_customers ,2,(customers_file));
	
	int num_ec_customers = 0;
	int num_bs_customers = 0;
	

	while (!feof(customers_file)) {
        	
 
        
	}
 
	fclose(customers_file);
 
    return 0;
}
