#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>

char* flag[] = { "OTH", "REJ", "RSTO", "RSTOS0", "RSTR", "S0", "S1", "S2", "S3", "SF", "SH", "SHR"}; //12 values

void fill_flag(char* sep, char value[]){

	char flag_value[10];
	
	int i = 0;

	while(*sep != ' '){

		flag_value[i] = *sep;		
		i++; sep++;

	}

	flag_value[i] = 0;

	for(int k = 0; k<12; k++){

		//printf("\nflag value: %s\n", flag_value);
		//printf("\nflag k: %s\n", flag[k]);

		if(strcmp(flag_value, flag[k]) == 0){

			//printf("\nflag value: %s\n", flag[k]);
			
			//c = k + '0';
			sprintf(value, "%d", k);

			}
	}

	//printf("\nstring: %s\n", value);

}

int fill_pattern(char pattern[], int pos, char* sep){

	while(*sep != ' '){

		pattern[pos] = *sep;		
		pos++; sep++;

	}

	return pos;

}

int generate_nsl_data(const char *file_name, const char *file_to_write, const char *pattern_class)
{

	char line[1024];
	char pattern[8000];

	const char *first_position = pattern_class; //first_position keeps first position from pointer pattern_class

    	if(file_name == NULL)
        	return 0; // return error

    	// no need to allocate memory for it
    	// if the insert_node is going to make a
    	// copy anyway
   
    	FILE *fp = fopen(file_name, "r");
	FILE *fp_write = fopen(file_to_write, "w");
    	
	if(fp == NULL)
    	{
        	fprintf(stderr, "Error opening file %s: %s\n", file_name,
                strerror(errno));
        	return 0;
    	}

	if(fp_write == NULL)
    	{
        	fprintf(stderr, "Error opening file %s: %s\n", file_to_write,
                strerror(errno));
        	return 0;
    	}

   
    	while(fgets(line, sizeof line, fp))
    	{

		int i = 0;
		char *sep;

		line[strcspn(line, "\n")] = 0;

		//long int src_ip = strtol(line, &sep, 0);

		sep = line;

		int blank_num = 0;

		while(*sep != 0){

			if(*sep == ' '){

				blank_num++;
				
				if(blank_num == 6){ //Duration

					i = fill_pattern(pattern, i, sep+1);

					//Service
					pattern[i] = ','; pattern[i+1] = '0';
					pattern[i+2] = ',';
					i = i+3;  

				}

				if(blank_num == 9){ //Flag

					char value [10];
					fill_flag(sep+1, value);
					
					int k = 0;					

					while(value[k] != 0){pattern[i] = value[k]; k++; i++;}

					pattern[i] = ','; i++;
					
				}

				if(blank_num == 10){ //src_bytes

					i = fill_pattern(pattern, i, sep+1);
					pattern[i] = ',';i++;
					
				}

				if(blank_num == 11){ //dst_bytes

					i = fill_pattern(pattern, i, sep+1);
					pattern[i] = ','; i++;
					
				}

				if(blank_num == 13){ //wrong fragment

					i = fill_pattern(pattern, i, sep+1);
					pattern[i] = ','; i++;
					
				}

				if(blank_num == 14){ //urgent

					i = fill_pattern(pattern, i, sep+1);
					pattern[i] = ','; i++;
					
				}

				if(blank_num == 15){ //hot

					i = fill_pattern(pattern, i, sep+1);
					pattern[i] = ','; i++;
					
				}

				if(blank_num == 16){ //num_failed_logins

					i = fill_pattern(pattern, i, sep+1);
					pattern[i] = ','; i++;
					
				}

				if(blank_num == 18){ //num_compromised

					i = fill_pattern(pattern, i, sep+1);
					pattern[i] = ','; i++;
					
				}

				if(blank_num == 20){ //su_attempted

					i = fill_pattern(pattern, i, sep+1);
					pattern[i] = ','; i++;
					
				}

				if(blank_num == 21){ //num_root

					i = fill_pattern(pattern, i, sep+1);
					pattern[i] = ','; i++;
					
				}

				if(blank_num == 22){ //num_file_creations

					i = fill_pattern(pattern, i, sep+1);
					pattern[i] = ','; i++;
					
				}

				if(blank_num == 23){ //num_shells

					i = fill_pattern(pattern, i, sep+1);
					pattern[i] = ','; i++;
					
				}

				if(blank_num == 24){ //num_access_files

					i = fill_pattern(pattern, i, sep+1);
					pattern[i] = ','; 

					//count
					pattern[i+1] = '0';
					pattern[i+2] = ',';

					i = i+3;

					//srv_count
					pattern[i] = '0';
					pattern[i+1] = ',';

					i = i+2;

					//dst_host_count
					pattern[i] = '0';
					pattern[i+1] = ',';

					i = i+2;

					//dst_host_srv_count
					pattern[i] = '0';
					pattern[i+1] = ',';

					i = i+2;

					//pattern class			
					while(*pattern_class != 0){
				
						pattern[i] = *pattern_class; 
						pattern_class++; 
						i++;

					}	

					
					
				}
					
			}

			sep++;		
			//fwrite(pattern , 1 , sizeof(strlen(pattern)) , fp_write);

			//memset(pattern, 0, sizeof(pattern));

		}

		pattern[i+1] = 0;
		fprintf (fp_write, "%s\n", pattern);
		memset(pattern, 0, sizeof(pattern));
		pattern_class = first_position;
		
		//pattern[i+1] = 0;
		//printf("\npattern: %s\n", pattern); printf("\nposition: %d\n", i);

		//break;

    	}

    fclose(fp);
    fclose(fp_write);
    return 1;
}

int generate_alert_data(const char *file_name, const char *file_to_write){

	char line[1024];
	char pattern[5000];

    	if(file_name == NULL)
        	return 0; // return error

	if(file_to_write == NULL)
        	return 0; // return error

    	// no need to allocate memory for it
    	// if the insert_node is going to make a
    	// copy anyway
   
    	FILE *fp = fopen(file_name, "r");
	FILE *fp_write = fopen(file_to_write, "w");
    	
	if(fp == NULL)
    	{
        	fprintf(stderr, "Error opening file %s: %s\n", file_name,
                strerror(errno));
        	return 0;
    	}

	if(fp_write == NULL)
    	{
        	fprintf(stderr, "Error opening file %s: %s\n", file_to_write,
                strerror(errno));
        	return 0;
    	}


	while(fgets(line, sizeof line, fp))
    	{
		int i = 0;
		char *sep;

		line[strcspn(line, "\n")] = 0;

		sep = line;

		int blank_num = 0;

		while(*sep != 0){

			if(*sep == ' '){

				blank_num++;

				if(blank_num == 1){

					i = fill_pattern(pattern, i, sep+1);
					pattern[i] = ' '; i++;

				}

				if(blank_num == 4){

					i = fill_pattern(pattern, i, sep+1);
					pattern[i] = ' '; i++;

				}

				if(blank_num == 5){

					i = fill_pattern(pattern, i, sep+1);
					
				}

			}

			sep++;
		}

		pattern[i] = 0;
		fprintf (fp_write, "%s\n", pattern);
		memset(pattern, 0, sizeof(pattern));	
	}

	fclose(fp);
    	fclose(fp_write);
    	return 1;

}

int main()
{
		
	generate_nsl_data("trafAld.list", "dataset.txt", "anomaly");
	generate_alert_data("trafAld.list", "alert_data.txt");
	
	return 0;
	
}
