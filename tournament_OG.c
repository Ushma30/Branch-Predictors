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
int p_bits = 4096;
int g_bits = 12;
uint32_t pht_o [4096];
uint32_t pht_g [4096];
int ghr[12];
int ghr_selector [12];
uint32_t selector [4096];
uint32_t pht_address_g;
uint32_t pht_address_o;
uint32_t ghr_selector_add;
uint32_t selector_add;
uint32_t lhr_address;
uint32_t pc_address;
uint32_t pc_address_g;
uint32_t pc_address_l;
uint32_t ghr_address;
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
      while(pow_count <= d) {
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

void use_gshare(trace_memory_read trace_data) {
	int i = 0;
	pc_address_g = trace_data.pc >> 2;
	pc_address_g = pc_address_g & (p_bits - 1);
	ghr_address = convertBinaryToDecimal(ghr);
	pht_address_g = pc_address_g ^ ghr_address;

    if (strcmp (trace_data.t_nt, "T") == 0) {
  	   
	    if((0 <= pht_g[pht_address_g]) && (pht_g[pht_address_g] <= ((power_func(2, bit_size)/2)-1))) {
			miss_predict++;
		}
		
		if((((power_func(2, bit_size))/2) <= pht_g[pht_address_g]) && (pht_g[pht_address_g] <= (power_func(2, bit_size)-1))) {
			correct_predict++;
		}
		if(pht_g[pht_address_g] < (power_func(2, bit_size)-1)) {
			pht_g[pht_address_g]++;
		}

		/* shifting array elements */
		for(i = 0; i < g_bits; i++)
	    {
	        ghr[i] = ghr[i + 1];
	    }
	    ghr[g_bits - 1] = 1;
	}

	if(strcmp (trace_data.t_nt, "N") == 0) {
		
	    if((0 <= pht_g[pht_address_g]) && (pht_g[pht_address_g] <= ((power_func(2, bit_size)/2)-1))) {
			correct_predict++;
		}
		
		if((((power_func(2, bit_size))/2) <= pht_g[pht_address_g]) && (pht_g[pht_address_g] <= (power_func(2, bit_size)-1))) {
			miss_predict++;
		}
		if(pht_g[pht_address_g] > 0) {
			pht_g[pht_address_g]--;
		}
		/* shifting array elements */
	    for(i = 0; i < g_bits; i++)
	    {
	        ghr[i] = ghr[i + 1];
	    }
	    ghr[g_bits - 1] = 0;	 	
	}
}

void use_one_level(trace_memory_read trace_data) {
	
	pht_address_o = trace_data.pc >> 2;
    pht_address_o = pht_address_o & (p_bits - 1);
    if (strcmp (trace_data.t_nt, "T") == 0) {
		if((0 <= pht_o[pht_address_o]) && (pht_o[pht_address_o] <= ((power_func(2, bit_size)/2)-1))) {
			miss_predict++;
		}
		
		if((((power_func(2, bit_size))/2) <= pht_o[pht_address_o]) && (pht_o[pht_address_o] <= (power_func(2, bit_size)-1))) {
			correct_predict++;
		}
		if(pht_o[pht_address_o] < (power_func(2, bit_size)-1)) {
			pht_o[pht_address_o]++;
		}
	}

	if(strcmp (trace_data.t_nt, "N") == 0) {
		if((0 <= pht_o[pht_address_o]) && (pht_o[pht_address_o] <= ((power_func(2, bit_size)/2)-1))) {
			correct_predict++;
		}
		
		if((((power_func(2, bit_size))/2) <= pht_o[pht_address_o]) && (pht_o[pht_address_o] <= (power_func(2, bit_size)-1))) {
			miss_predict++;
		}
		if(pht_o[pht_address_o] > 0) {
			pht_o[pht_address_o]--;
		}
	}

}

int gshare(trace_memory_read trace) {
	
    int gcp = 0;
    pc_address_g = trace.pc >> 2;
    pc_address_g = pc_address_g & (p_bits - 1);
    ghr_address = convertBinaryToDecimal(ghr);
	pht_address_g = pc_address_g ^ ghr_address;
    if (strcmp (trace.t_nt, "T") == 0) {
		
		if((((power_func(2, bit_size))/2) <= pht_g[pht_address_g]) && (pht_g[pht_address_g] <= (power_func(2, bit_size)-1))) {
			gcp++;
		}		
	}

	if(strcmp (trace.t_nt, "N") == 0) {
		if((0 <= pht_g[pht_address_g]) && (pht_g[pht_address_g] <= ((power_func(2, bit_size)/2)-1))) {
			gcp++;
		}
	}
	return gcp;
}


//**********************************************************************
// Function Name: two_bit_local							               *
// Description: Reads the type of instruction executed in the trace    *
// Input: file						  								   *
// Return: structure of trace_memory_read					   		   *
//**********************************************************************
int one_level(trace_memory_read trace) {
	
    int ocp = 0;
    pht_address_o = trace.pc >> 2;
    pht_address_o = pht_address_o & (p_bits - 1);
    if (strcmp (trace.t_nt, "T") == 0) {
		
		if((((power_func(2, bit_size))/2) <= pht_o[pht_address_o]) && (pht_o[pht_address_o] <= (power_func(2, bit_size)-1))) {
			ocp++;
		}
	}

	if(strcmp (trace.t_nt, "N") == 0) {
		if((0 <= pht_o[pht_address_o]) && (pht_o[pht_address_o] <= ((power_func(2, bit_size)/2)-1))) {
			ocp++;
		}
	}
	return ocp;
}


//**********************************************************************
// Function Name: tournament							               *
// Description: Reads the type of instruction executed in the trace    *
// Input: file						  								   *
// Return: structure of trace_memory_read					   		   *
//**********************************************************************

void tournament (trace_memory_read trace) {

	pc_address = trace.pc >> 2;
    pc_address = pc_address & (p_bits - 1);
    ghr_selector_add = convertBinaryToDecimal(ghr_selector);
	selector_add = pc_address ^ ghr_selector_add;

	count++;
    int i = 0;
	int gvar = 0;
	int ovar = 0;

    gvar = gshare(trace);
    ovar = one_level(trace);
    
    
    if ((0 <= selector[selector_add]) && (selector[selector_add] <= ((power_func(2, bit_size)/2)-1))) {
		use_one_level(trace);
	}
	if((((power_func(2, bit_size))/2) <= selector[selector_add]) && (selector[selector_add] <= (power_func(2, bit_size)-1))) {
		use_gshare(trace);
	}

	if ((gvar == 1) && (ovar == 0) && (selector[selector_add] < (power_func(2, bit_size)-1))) {
    	selector[selector_add]++;
    }
    if ((ovar == 1) && (gvar == 0) && (selector[selector_add] > 0)) {
    	selector[selector_add]--;
    }

    if (strcmp (trace.t_nt, "T") == 0) {
    	
    	for(i = 0; i < g_bits; i++) {
	        ghr_selector[i] = ghr_selector[i+1];
	    }
	    ghr_selector[g_bits - 1] = 1;

	}

	if(strcmp (trace.t_nt, "N") == 0) {
		
		for(i = 0; i < g_bits; i++) {
	        ghr_selector[i] = ghr_selector[i+1];
	    }
	    ghr_selector[g_bits - 1] = 0;
	 	
	}

}
    


void main(int argc, char *argv[]){
	
	if  (argc != 2 ) { 
		printf("Enter the number of bits for Two Level Local Branch Prediction:\n./(executable name) Bit Size: 2|3|4|6|8\n");
        exit(0);
    }
	else {

		int i = 0;
		int j = 0;

		bit_size = atoi(argv[1]);

		for (i = 0; i < p_bits; i++) {
			selector [i] = 0;
		}
		for (i = 0; i < p_bits; i++) {
			pht_g [i] = 0;
		}
		for (i = 0; i < g_bits; i++){
			ghr[i] = 0;
		}
		for (i = 0; i < g_bits; i++){
			ghr_selector[i] = 0;
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
	        tournament(trace_val);
		}
		printf("%d\n", miss_predict);
		printf("%d\n", correct_predict);
		printf("%d\n", count);
		printf("Tournament Branch Correct Prediction Percentage: %1.3f\n", (((double)correct_predict/count))*100);
		printf("Tournament Branch Miss Prediction Percentage: %1.3f\n", (((double)miss_predict/count))*100);
	}

}
