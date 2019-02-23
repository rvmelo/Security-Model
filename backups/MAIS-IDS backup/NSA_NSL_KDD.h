#ifndef NSA
#define NSA


#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<string.h>
#include <stdbool.h>
#include <ctype.h>
#include "Data.h"

// Algorithm configuration

#define SIZE 19 //Number of attributes for traffic data
#define MAX_SELF 150
#define MIN_DIST 1.5 // Original value = 0.05


//struct Node* self_dataset;
	
//Declared functions

double binaryToDouble(const char binary[]){
  int bi,i;
  int len = 0;
  int dot = -1;
  double result = 0;

  for(bi = 0; binary[bi] != '\0'; bi++){
    if(binary[bi] == '.'){
      dot = bi;
    }
    len++;
  }
  if(dot == -1)
    dot=len;

  for(i = dot; i >= 0 ; i--){
    if (binary[i] == '1'){
      result += (double) pow(2,(dot-i-1));
    }
  }
  for(i=dot; binary[i] != '\0'; i++){
    if (binary[i] == '1'){
      result += 1.0/(double) pow(2.0,(double)(i-dot));
    }
  }
  return result;
}

double doubleToBinary(double f){
	
	int  integral, binaryInt = 0, i = 1;
    double  binaryFract = 0, k =0.1f, fractional, temp1, binaryTotal;

    //printf("***** Convert float to binary *******\n");
    //printf("\nEnter float value : ");
    //scanf("%lf",&f);

    //Separating the integral value from the floating point variable
    integral = (int)f;

    //Separating the fractional value from the variable
    fractional = f - (int)f;

    //Loop for converting decimal to binary
    while(integral>0)
    {
        binaryInt = binaryInt + integral % 2 * i;
        i = i * 10;
        integral = integral / 2;
    }

    //Loop for converting Fractional value to binary
    while(k>0.00000001)
    {
        temp1 = fractional *2;
        binaryFract = binaryFract+((int)temp1)*k;
        fractional = temp1 - (int)temp1;
        k = k / 10;
    }

    //Combining both the integral and fractional binary value.
    
    binaryTotal = binaryInt +binaryFract;
    return binaryTotal;
    
    //printf(" \nbinary equivalent = %lf\n\n\n\n\n", binaryTotal);
	
	}

double euclidian_distance(double c1[SIZE], double c2[SIZE]){
	
	double sum = 0.0;
		
	for(int i = 0; i<SIZE; i++){
		
		sum += pow((c1[i] - c2[i]),2);
		
		}

        if(sqrtf(sum) <= 0.0){ 

		/*printf("\nEUCLIDEAN DISTANCE EQUAL TO ZERO!!\n\n");
		
		for(int i = 0; i<SIZE; i++)
			printf("%lf ", c1[i]);
		
		printf("\n");

		for(int i = 0; i<SIZE; i++)
			printf("%lf ", c2[i]);

		printf("\n");*/
              

	}   
		
	return sqrtf(sum);
	
	}

bool equal_patterns(double pattern[SIZE], double pattern_2[SIZE]){

	for(int i = 0; i<SIZE; i++){

             if(pattern[i] != pattern_2[i])
		return false;

	}

	return true;

}

bool check_pattern_list(struct Node* list, double pattern[SIZE]){ //Check if pattern is in list
     
      int counter = 0;
     
      while(list != NULL){

       for(int i = 0; i<SIZE; i++){
       	 
         if(list->pattern[i] == pattern[i])
	    	counter++;

       }

       if(counter == SIZE)
	 return true;
       else
         counter = 0; 
       
       list = list->next;

     }

     return false; 

}

void insert_list(struct Node** list, double detector[SIZE]){
	
	struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
		
	for(int i = 0; i < SIZE; i++){
		
		temp -> pattern[i] = detector[i];
		temp -> bitstring[i] = doubleToBinary(detector[i]);
		
		}

	temp->memory_cells = NULL;
	temp->best_detector = NULL; 

        temp->affinity = 0.0;
        temp->fitness = 0.0;
        temp->radius = 0.0;
        temp->cost = 0.0;

        temp->counter = 0;
        temp->correct = 0;
        temp->confirm = 0;
									
	temp -> next = *list;
	*list = temp;
			
	}

void insert_memorycell(struct Node** list, double pattern[SIZE]) {
    
    struct Node* cell = (struct Node*)malloc(sizeof(struct Node));
    
    for(int i = 0; i < SIZE; i++){
		
		cell -> pattern[i] = pattern[i];
		cell -> bitstring[i] = doubleToBinary(pattern[i]);
                
		}

    cell->memory_cells = NULL;
    cell->best_detector = NULL;

    cell->affinity = 0.0;
    cell->fitness = 0.0;
    cell->radius = 0.0;
    cell->cost = 0.0;

    cell->counter = 0;
    cell->correct = 0;
    cell->confirm = 0;
     
    cell->next = (*list)->memory_cells;
    (*list)->memory_cells = cell;
}

void insert_best_detector(struct Node** list, double pattern[SIZE]) {
    
    struct Node* cell = (struct Node*)malloc(sizeof(struct Node));
    
    for(int i = 0; i < SIZE; i++){
		
		cell -> pattern[i] = pattern[i];
		cell -> bitstring[i] = doubleToBinary(pattern[i]);
		
		}

    cell->memory_cells = NULL;
    cell->best_detector = NULL;

    cell->affinity = 0.0;
    cell->fitness = 0.0;
    cell->radius = 0.0;
    cell->cost = 0.0;

    cell->counter = 0;
    cell->correct = 0;
    cell->confirm = 0;
     
    cell->next = (*list)->best_detector;
    (*list)->best_detector = cell;
}

void delete_list(struct Node** head_ref)
{
   /* deref head_ref to get the real head */
   struct Node* current = *head_ref;
   struct Node* next;

   // i = 0;
 
   while (current != NULL) 
   {

       //printf("\n i value: %d\n", i);

       next = current->next;
       free(current);
       current = next;

       //i++;
   }
   
   /* deref head_ref to affect the real head back
      in the caller. */
   *head_ref = NULL;
}

void delete_node_list(struct Node** list){

	struct Node* temp = *list;

	while(*list != NULL){

		//printf("\n i value: %d\n", i);

		delete_list(&((*list)->memory_cells));
		delete_list(&((*list)->best_detector));

		*list = (*list)->next;	
	}

	*list = temp;

	delete_list(list);

}
	
void print_list(struct Node * list){
	
	struct Node* temp = list;
	int k = 0;
			
	while (temp != NULL){
		
		k += 1;
				
		for(int i = 0; i<SIZE; i++){
		
		if(i == 0)
		
			printf("Pattern %d: [%.2lf", k, temp->pattern[i]);
			//printf("Detector %d: [%.2lf", k, temp->bitstring[i]);
		
		else if ( (i > 0) && (i < SIZE - 1))
		
			printf(", %.2lf ", temp->pattern[i]);
			//printf(", %.2lf ", temp->bitstring[i]);
			
		else
		
			printf(", %.2lf] ", temp->pattern[i]);
			//printf(", %.2lf] ", temp->bitstring[i]);
		}
			
			//cost and affinity values
			
			printf("radius: %lf ", temp->radius);
			
			printf("fitness: %lf ", temp->fitness);

			printf("correct: %d ", temp->correct);
                         
                        //printf("confirm: %d ", temp->confirm);

                        //printf("class: %d", temp->instance_class);

                        printf("counter: %d ", temp->counter);
			
			printf("\n");
		
		temp = temp-> next;
			
	}	
	
}	
	
int GetFileLines(char const * name){ //Functions that use string literals should pass char const * variable
	
	FILE * fPointer;
	fPointer = fopen(name, "r");	
		
	int ch;	
	int lines = 0; 
				
	while(!feof(fPointer)){ 
				
		ch = fgetc(fPointer); 
		
		if(ch == '\n') //Filling attribute
																					
			lines += 1; 
																				
		}
		
		fclose(fPointer);		
		
		return lines+1;
			
	}	

double Normalization(double value, double min, double max){
		
	double normalValue = 0.0;
	
	if(max - min == 0.0)
		normalValue = 0.0;
	else
		normalValue = (value-min)/(max - min);
	
	return normalValue;
	
	}	
	
double * random_vector(){
	
	static double r[SIZE];
					
		for(int i = 0; i < SIZE; i++){
			
			r[i] = (double)rand() / (double)RAND_MAX;
			
			}
					
		return r;					
	}			
		
bool matches_detectors(struct Node* detectors, double pattern[SIZE]){
	
	struct Node* temp = detectors;
		
	while (temp != NULL){
						
		double dist = euclidian_distance(pattern, temp->pattern); //each element in dataset is a pattern to be compared
						
		if(dist < temp->radius)			
			return true;
							
		temp = temp->next;
											
		}
	
	   return false;
	   	   
	}

bool euclidian_distance_match_radius(struct Node* detectors, double pattern[SIZE]){

	while (detectors != NULL){
						
		double dist = euclidian_distance(pattern, detectors->pattern); 
								
		if(dist < detectors->radius)			
			return true;
			
		detectors = detectors->next;
								
		}
	
	   return false;
		
}
	
bool euclidian_distance_match_list(struct Node* detectors, double detector[SIZE],  double min_dist){
			
	while (detectors != NULL){
						
		double dist = euclidian_distance(detector, detectors->pattern); 
								
		if(dist <= min_dist)			
			return true;
			
		detectors = detectors->next;
								
		}
	
	   return false;
	
	}		

double GetPatternAttribute(char value[25], int n){
	
	char *ptr;
        double ret = 0.0;
        double patternAttribute;
                
        ret = strtod(value, &ptr);
                        
        if(n == 0) //Duration
			patternAttribute = Normalization(ret, 0, 58329);
									
		else if (n == 1) //Service
			patternAttribute = Normalization(ret, 0, 69);
					
		else if (n == 2) //Flag
			patternAttribute = Normalization(ret, 0, 10);
			
			else if (n == 3 || n == 4){ //src_byte/dst_bytes
																
				  if (n == 3){
					  
					if(ret > 0)
					
						patternAttribute = Normalization(log10(ret), 0, 9.14);
							else
						patternAttribute = Normalization(ret, 0, 9.14);						
					
					}
						else{ //Attribute number == 5 (i+1)
					
					if(ret > 0)
						patternAttribute = Normalization(log10(ret), 0, 9.14);
							else
						patternAttribute = Normalization(ret, 0, 9.14);
						
						}
					
				
				} else if (n == 5) //wrong_fragment
					patternAttribute = Normalization(ret, 0, 3);
					
				  else if (n == 6) //urgent
					patternAttribute = Normalization(ret, 0, 14);
					
				  else if (n == 7) //hot
					patternAttribute = Normalization(ret, 0, 101);
					
				  else if (n == 8) //num_failed_logins
					patternAttribute = Normalization(ret, 0, 5);	
					
				  else if (n == 9) //num_compromised
					patternAttribute = Normalization(ret, 0, 9);	
				
				  else if (n == 10) //su_attempted
					patternAttribute = Normalization(ret, 0, 2);
						
				  else if (n == 11) //num_root
					patternAttribute = Normalization(ret, 0, 7468);
					
				  else if (n == 12) //num_file_creations
					patternAttribute = Normalization(ret, 0, 100);
					
				  else if (n == 13) //num_shells
					patternAttribute = Normalization(ret, 0, 5);
					
				  else if (n == 14) //num_access_files
					patternAttribute = Normalization(ret, 0, 9);
					
				  else if (n == 15) //count
					patternAttribute = Normalization(ret, 0, 511);
					
				  else if (n == 16) //srv_count
					patternAttribute = Normalization(ret, 0, 511);
					
				  else if (n == 17) //dst_host_count
					patternAttribute = Normalization(ret, 0, 255);
					
				  else if (n == 18) //dst_host_srv_count
					patternAttribute = Normalization(ret, 0, 255);
										
					return patternAttribute;
	
	}
	
void set_pattern(double * pattern, FILE** pointer){ //Gets pattern from self dataset (training data) file
		
	int ch = 0;
	
	int i = 0;
	char attribute[25];
	
	int k = 0;
           	
	while(ch != '\n' && !feof(*pointer)){
		
		ch = fgetc(*pointer); //printf("%d", ch);
				
		if(ch != ','){	
					
			attribute[i] = ch;
			i += 1;
						
		}
		
		else{
			
			attribute[i] = '\0';
			
			if(k < SIZE)			
				pattern[k] = GetPatternAttribute(attribute, k);
			
			i = 0;
			
			memset(attribute, 0, sizeof(attribute));
			
			k += 1;
			
			}
		
		}
					
	}

char * GetInstanceClass(FILE** pointer){ //If record in file is normal or an anomaly
		
	int chPosition = 0;	
	int ch = 0;
	
	static char lastAttribute[25];
						
	while(ch != '\n' && !feof(*pointer)){ 
				
		ch = fgetc(*pointer); //printf("%c", ch);
		
		if(ch != ',' || ch == '\n'){ //Filling attribute
						
			lastAttribute[chPosition] = ch;
			chPosition += 1;
			
			if(ch == '\n'){ // Last attribute from pattern -> anomaly/normal
								
				lastAttribute[chPosition] = '\0';				
				chPosition = 0;
               			
									
				}
						
			}
			
			else{ //Attribute is full
								
				lastAttribute[chPosition] = '\0';
				chPosition = 0;
																		
				memset(lastAttribute, 0, sizeof(lastAttribute));
												
			}	
			
		
		}
		
                //printf("Size of stlren: %d\n", (int)strlen(lastAttribute));
                							
		return lastAttribute; 	
}			

void generate_dataset(struct Node** dataset, char* file_name){

	int lines = 0;
	int lineCount = 0;	

	double pattern[SIZE] = {0.0};
	
	FILE* f_Data_Pointer = fopen(file_name, "r");
	FILE* f_Class_Pointer = fopen(file_name, "r");
	
	lineCount = GetFileLines(file_name);

	printf("%s has %d lines \n", file_name, lineCount);

	while(lines < lineCount){
	 			
		set_pattern(pattern, &f_Data_Pointer);
      
		insert_list(dataset, pattern);
				
		if(strncmp(GetInstanceClass(&f_Class_Pointer),"normal", 6) == 0) // false -> normal / true -> anomaly
                //if((int)strlen(GetInstanceClass(&f_Class_Pointer)) == 7 || (int)strlen(GetInstanceClass(&f_Class_Pointer))== 8)
			(*dataset)->instance_class = false;
		else
			(*dataset)->instance_class = true;
   	  
		lines +=1;
	  	  	   
	}
			
}

void print_detectors_performance(struct Node* detectors, struct Node* dataset){

	struct Node* temp = dataset;
	
	int true_positive = 0;
        int true_negative = 0;
	int self_antigen_num = 0;
	int non_self_antigen_num = 0;

	int false_positive = 0;
	int false_negative = 0;

	while(temp != NULL){

		if(temp->instance_class)
			non_self_antigen_num++;
		else
			self_antigen_num++;

		temp = temp->next;

	}

	int total = non_self_antigen_num + self_antigen_num;

	while(dataset != NULL){

		if(euclidian_distance_match_radius(detectors, dataset->pattern)){

			if(dataset->instance_class) //dataset is an anomaly and it's identified as anomaly
				true_positive++;

			if(!dataset->instance_class) //dataset is normal but it's identified as anomaly
				false_positive++;
			

		}else{

			if(!dataset->instance_class) //dataset is normal and it's identified as normal
				true_negative++;

			if(dataset->instance_class) //dataset pattern is an anomaly but it's identified as normal
				false_negative++;
		}

		
		dataset = dataset->next;

	}

	int correct = true_positive+true_negative;

	if(correct > 0)
		printf("\nAccuracy: %.2lf%%\n", ((double)correct/((double)correct+(double)false_positive+(double)false_negative))*100);
	else
		printf("\nAccuracy 0.0%%\n");

	if(false_positive > 0)
		printf("False postive: %.2lf%%\n", (((double)false_positive/(double)(true_negative+false_positive))*100));
	else
		printf("False positive: 0.0%%\n");

	if(false_negative > 0)
		printf("False negative: %.2lf%%\n", (((double)false_negative/(double)(true_positive+false_negative))*100));
	else
		printf("False negative: 0.0%%\n");


}

double get_detectors_accuracy(struct Node* detectors, struct Node* dataset){

	struct Node* temp = dataset;
	
	int true_positive = 0;
        int true_negative = 0;
	int self_antigen_num = 0;
	int non_self_antigen_num = 0;

	int false_positive = 0;
	int false_negative = 0;

	double acc = 0.0;

	while(temp != NULL){

		if(temp->instance_class)
			non_self_antigen_num++;
		else
			self_antigen_num++;

		temp = temp->next;

	}

	int total = non_self_antigen_num + self_antigen_num;

	while(dataset != NULL){

		if(euclidian_distance_match_radius(detectors, dataset->pattern)){

			if(dataset->instance_class) //dataset is an anomaly and it's identified as anomaly
				true_positive++;

			if(!dataset->instance_class) //dataset is normal but it's identified as anomaly
				false_positive++;
			

		}else{

			if(!dataset->instance_class) //dataset is normal and it's identified as normal
				true_negative++;

			if(dataset->instance_class) //dataset pattern is an anomaly but it's identified as normal
				false_negative++;
		}

		
		dataset = dataset->next;

	}

	int correct = true_positive+true_negative;

	if(correct > 0)
		acc = ((double)correct/((double)correct+(double)false_positive+(double)false_negative))*100;	
	
	return acc;


}

void set_detector_radius(struct Node** detectors, struct Node* self_dataset){
		
	struct Node* temp = self_dataset;
	
	double min_dist = 1000;

	if(!equal_patterns(temp->pattern, (*detectors)->pattern))
		min_dist = euclidian_distance(temp->pattern, (*detectors)->pattern);
        	
	//min_dist = euclidian_distance(temp->pattern, (*detectors)->pattern);
			
	while(temp != NULL){ // temp is self_dataset
		
	   if(!equal_patterns(temp->pattern, (*detectors)->pattern) && min_dist > euclidian_distance(temp->pattern, (*detectors)->pattern))
			min_dist = euclidian_distance(temp->pattern, (*detectors)->pattern);
			
	   temp = temp->next;	
		
	}
		
	(*detectors)->radius = min_dist;	
		
}

void generate_detectors(struct Node** detectors, struct Node* self_dataset, int max_detectors, double min_dist){
	
	int i = 0;
			
	double * detector;
		
	while(i < max_detectors){
		
		detector = random_vector();
			
			if(!euclidian_distance_match_list(*detectors, detector, 0.0)){
												
				insert_list(detectors, detector);
				set_detector_radius(detectors, self_dataset);								
				i++;
				
				}		
		}
	
	}			
		

void negative_selection(struct Node** detectors, struct Node* self_dataset, int max_detectors){
				
    generate_detectors(detectors, self_dataset, max_detectors, MIN_DIST);
    
			
	}


#endif


