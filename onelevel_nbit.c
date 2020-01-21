#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>

typedef struct {
    uint32_t pc;
    char* t_nt;
} trace_memory_read;

//Global variables declarations:
uint32_t pht [1024];
uint32_t pht_address;
uint32_t bit_size;
int miss_predict = 0;
int correct_predict = 0;
int count = 0;

//**********************************************************************
// Function Name: read_file 							               *
// Description: Reads the type of instruction executed in the trace    *
// Input: file						  								   *
// Return: structure of trace_memory_read					   		   *
//**********************************************************************
trace_memory_read read_file(FILE *file) {
    
    /* Data read from the file*/
    char max_size[1000000];
    int* op;
    char* max_size_string = max_size;
    trace_memory_read data_trace;

	while (fgets(max_size, 1000000, file) != EOF) {
		op = strtok(max_size_string, " ");
		data_trace.pc = (uint32_t)strtol(op, NULL, 10);
		data_trace.t_nt = strtok(NULL, " \n");

     return data_trace;   
    }
    
}

int power_func(int c, int d)
{
      int value = 1;
      int pow_count = 1;
      while(pow_count <= d) 
      {
            value = value * c;
            pow_count++;
      }
      return value;
}

//**********************************************************************
// Function Name: one_bit_predict 						               *
// Description: Function to perform One Level Branch Prediction    	   *
// Input: address of the PHT and the trace structure				   *
// Return: void					   		   							   *
//**********************************************************************
void one_bit_predict(trace_memory_read trace) {
	
    count++;
    pht_address = trace.pc >> 2;
    pht_address = pht_address & 1023;
    if (strcmp (trace.t_nt, "T") == 0) {
		if((0 <= pht[pht_address]) && (pht[pht_address] <= ((power_func(2, bit_size)-1)/2))) {
			miss_predict++;
		}
		
		if((((power_func(2, bit_size))/2) <= pht[pht_address]) && (pht[pht_address] <= (power_func(2, bit_size)-1))) {
			correct_predict++;
		}
		if(pht[pht_address] < (power_func(2, bit_size)-1)) {
			pht[pht_address]++;
		}
	}

	if(strcmp (trace.t_nt, "N") == 0) {
		if((0 <= pht[pht_address]) && (pht[pht_address] <= ((power_func(2, bit_size)-1)/2))) {
			correct_predict++;
		}
		
		if((((power_func(2, bit_size))/2) <= pht[pht_address]) && (pht[pht_address] <= (power_func(2, bit_size)-1))) {
			miss_predict++;
		}
		if(pht[pht_address] > 0) {
			pht[pht_address]--;
		}
	}
}

void main(int argc, char *argv[]){
	if  (argc != 2 ) { 
		printf("Enter the number of bits for One Level Branch Prediction:\n./(executable name) Bit Size: 2|3|4|6|8\n");
        exit(0);
    }
    else {
    	int i = 0;
    	bit_size = atoi(argv[1]);
		for (i = 0; i < 1023; i++){
			pht [i] = 0;
		}
		FILE *file;
		char *mode = "r";
		FILE *pfout;

		//opening file for reading
		file = fopen("branch-trace-gcc.trace",mode);
		if (file == NULL) {
			printf("Can't open input file\n");
			return(0);
		}

		trace_memory_read trace_val;
		while (1) {
			trace_val = read_file(file);
			if (trace_val.t_nt == NULL)
	            break;
	        
			one_bit_predict(trace_val);	
		}
		printf("%d\n", miss_predict);
		printf("%d\n", correct_predict);
		printf("%d\n", count);
		printf("One Level Branch Correct Prediction Percentage: %1.3f\n", (((double)correct_predict/count))*100);
		printf("One Level Branch Miss Prediction Percentage: %1.3f\n", (((double)miss_predict/count))*100);
    }
	
}
