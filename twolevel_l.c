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
uint32_t lhr [128][10];
uint32_t pht_address;
uint32_t lhr_address;
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

int convertBinaryToDecimal(int bin[])
{
	int decimalNumber = 0, i = 0, remainder;
	int digit_count=10; 
	int a = 0;
	for(i = (digit_count - 1); i >= 0; i--) {
		decimalNumber = (bin[i] * power_func(2, a)) + decimalNumber;
		a++;
	}
	return decimalNumber;
}

//**********************************************************************
// Function Name: two_bit_local							               *
// Description: Reads the type of instruction executed in the trace    *
// Input: file						  								   *
// Return: structure of trace_memory_read					   		   *
//**********************************************************************
void two_bit_local(trace_memory_read trace) {
	
    count++;
    int i = 0;
    lhr_address = trace.pc >> 2;
    lhr_address = lhr_address & 127;
    pht_address = convertBinaryToDecimal(lhr[lhr_address]);

    if (strcmp (trace.t_nt, "T") == 0) {
    	
	    if(pht[pht_address] == 0 || pht[pht_address] == 1) {
			miss_predict++;
		}
		
		if(pht[pht_address] == 2 || pht[pht_address] == 3) {
			correct_predict++;
		}
		if(pht[pht_address] < 3) {
			pht[pht_address]++;
		}
		for(i = 0; i < 10; i++)
	    {
	        lhr[lhr_address][i] = lhr[lhr_address][i + 1];
	    }
	    lhr[lhr_address][9] = 1;

	}

	if(strcmp (trace.t_nt, "N") == 0) {
		
		if(pht[pht_address] == 0 || pht[pht_address] == 1) {
			correct_predict++;
		}
		
		if(pht[pht_address] == 2 || pht[pht_address] == 3) {
			miss_predict++;
		}
		if(pht[pht_address] > 0) {
			pht[pht_address]--;
		}
		for(i = 0; i < 10; i++)
	    {
	        lhr[lhr_address][i] = lhr[lhr_address][i + 1];
	    }
	    lhr[lhr_address][9] = 0;
	 	
	}
}
    

void main(){
	
	int i = 0;
	int j = 0;

	for (i = 0; i < 1024; i++) {
		pht [i] = 0;
	}
	for (i = 0; i < 128; i++) {
		for (j = 0; j < 10; j++) {
			lhr [i][j] = 0;
		}
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
	while (1){
		trace_val = read_file(file);
		if (trace_val.t_nt == NULL)
            break;
		two_bit_local(trace_val);	
	}
	printf("%d\n", miss_predict);
	printf("%d\n", correct_predict);
	printf("%d\n", count);
	printf("Two Level Local Branch Correct Prediction Percentage: %1.3f\n", (((double)correct_predict/count))*100);
	printf("Two Level Local Branch Miss Prediction Percentage: %1.3f\n", (((double)miss_predict/count))*100);

}
