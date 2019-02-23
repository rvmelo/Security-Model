#ifndef CLONALG
#define CLONALG

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<string.h>
#include <stdbool.h>
#include "NSA_NSL_KDD.h"
#include "SortingLists.h"


//Algorithm configuration

#define MAX_GENS 100
#define POP_SIZE 100
#define CLONE_FACTOR 0.1
#define NUM_RAND 2
#define BITS_PER_PARAM 8

int list_size(struct Node * list){
	
	struct Node* temp = list;
	
	int size = 0;
			
	while (temp != NULL){
				
		size+= 1;
		
		temp = temp-> next;
			
	}
	
	return size;		
}

double objective_function(double vector[SIZE]){
		
	double sum = 0.0;
		
	for(int i = 0; i < SIZE; i++){
		
		sum += pow(vector[i],2);
			
		}
							
	return sum;
		
	}
	
void point_mutation(struct Node** clones, double * bitstring, double rate){ 
	
	char bits[BITS_PER_PARAM];
	char bit;
		
	double pattern[SIZE];
	
	for(int i = 0; i<SIZE; i++){//bitstring of 19 arguments
				
		sprintf(bits,"%lf", bitstring[i]);
					
		for(int j = 2; bits[j] != '\0'; j++){
			
			  if(bits[j] == '.')
				continue;
				
		      if(((double)rand() / (double)RAND_MAX) < rate){ 
				  
				  if(bits[j] == '1')
						bit = '0';
				     else
						bit = '1';
						
				  bits[j] = bit;
				  
				  }
				  				  							  			
			}
						
			pattern[i] = round(binaryToDouble(bits)*100)/100;
									
			memset(bits, 0, sizeof(bits));
							
		}
			
	for(int i = 0; i<SIZE; i++){
		
		(*clones)->pattern[i] = pattern[i];
		(*clones)->bitstring[i] = doubleToBinary(pattern[i]);
		
		}
		
	}

void normalize_affinity(struct Node** list){
	
	double affinity_max = 0.0;
	double affinity_min = 0.0;
	
	struct Node* temp = *list;
	
	affinity_max = temp->affinity;
	
	while(temp != NULL){ //Gets affinity value from last element from list
		
		affinity_min = temp->affinity;
		
		temp = temp->next;
		
		}
		
		temp = *list;
		
	while(*list != NULL){
		
		(*list)->affinity = Normalization((*list)->affinity, affinity_min, affinity_max);
		*list = (*list)->next;
		
		}
		
		*list = temp;	
	
	}		


void evaluate(struct Node** list){
		
	struct Node* temp = *list;
	
	while (*list != NULL){
					
		(*list)->cost = objective_function((*list)->pattern);
			
		*list = (*list)-> next;
		
		}
		
	*list = temp;
	
	}

void reduce_distance_point(double detector[], double best[]){

	/*printf("\n\nINSIDE JudgmentInstance: \n\n");

		for(int i = 0; i<SIZE; i++)
			printf("%lf ", detector[i]);
		printf("\n");*/	

	double p = (double)rand()/(double)RAND_MAX;
	
        for(int i = 0; i < SIZE; i++)
		detector[i] = detector[i] + (best[i] - detector[i])*p;
	
	/*printf("\n\nINSIDE JudgmentInstance 2: \n\n");

		for(int i = 0; i<SIZE; i++)
			printf("%lf ", detector[i]);
		printf("\n");*/

}

double random_gaussian(){
	
	double u1, u2, w, mean, stdev;
	
	u1 = 0.0; u2 = 0.0; w = 0.0; mean = 0.0; stdev = 1.0; // mean = 0.5 stdev = 0.125 to be between 0 and 1
	
	do{
			
		u1 = 2 * (double)rand() / (double)RAND_MAX -1;
		u2 = 2 * (double)rand() / (double)RAND_MAX -1;
		w = u1 * u1 + u2 * u2;
		
		} while (w >= 1);
		
		if(w != 0)
			w = sqrt((-2.0 * log(w))/w);
		else
			w = 0;
			
	return mean + (u2 * w) * stdev;
	
	}	

void mutate(double beta, double pattern[], double fitness){
	
	double alpha = 0.0;
	double aux = 0.0;
	
	//static double child[SIZE];
	
	/*for(int i = 0; i < SIZE; i++)
		child[i] = parent[i];*/
				
	for(int i = 0; i < SIZE; i++){
		
		if(fitness < 0) //HERE!!!!!!
			fitness = 0;
				
		alpha = (1.0/beta) * exp(-fitness); // mutation_rate
				
		do{				
						
			aux = pattern[i] + alpha * random_gaussian();
							
		}while(aux < 0 || aux > 1); //Between 0 and 1
		
		if(pattern[i] > 0.0) pattern[i] = aux;
				
		}
		
	//return child;
	
	}			
	
void hypermutate(struct Node** clones){
					
	double pattern[SIZE] ={0.0};
	
	int times = 0;
	
	//printf("\neuclidian distance: %.2lf\n", euclidian_distance((*clones)->pattern, (*clones)->best_detector->pattern));
	
	do{
	
	for(int i = 0; i< SIZE; i++)
		pattern[i] = (*clones)->pattern[i];
		
	double euclidian_d = euclidian_distance((*clones)->pattern, (*clones)->best_detector->pattern);	
											
	while(euclidian_distance((*clones)->pattern, (*clones)->best_detector->pattern) >= euclidian_d && euclidian_d > 0.0){
			
		for(int i = 0; i< SIZE; i++)
			(*clones)->pattern[i] = pattern[i]; 
						
		//mutate(100, (*clones)->pattern, (*clones)->fitness);
		reduce_distance_point((*clones)->pattern, (*clones)->best_detector->pattern);
										
		}
		
		times++;
	
	  }while(times < 1);
	  
	  //printf("\neuclidian distance 2: %.2lf\n", euclidian_distance((*clones)->pattern, (*clones)->best_detector->pattern));		
		
	}
		
double * get_min_cost(struct Node** list){
	
	double cost = 0.0;
	
	static double vector[SIZE];
	
	struct Node* temp = *list;
	
	cost = temp->cost;
	
		while (*list != NULL){
		
			if((*list)->cost < cost){
			
				for(int i = 0; i<SIZE; i++)
					vector[i] = (*list)->pattern[i];
					
				cost = (*list)->cost;
				
				}		
		
		*list = (*list)-> next;
		
		}
		
		*list = temp;
		
		return vector;
	
	}		
	
#endif

