#ifndef aiNet_Algo
#define aiNet_Algo


#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<string.h>
#include <stdbool.h>
#include "CLONALG_NSL_KDD.h"
#include "SortingLists.h"

//Algorithm configuration

#define MAX_GENS_2 150
#define POP_SIZE_2 20
#define NUM_CLONES 10
#define BETA 100
#define AFF_THRESHOLD 0.1 //calculated as a percentage of the size of the problem space -> 0.05 / Original value -> 0.1 

struct Node * progeny;
struct Node * neighbors;

bool cell_equal(double v1[SIZE], double v2[SIZE]){
	
	for(int i = 0; i < SIZE; i++){
		
		if(v1[i] != v2[i])
			return false;
		
		}
		
		return true;
	
	}

double avarage_cost(struct Node * list){
	
	struct Node* temp = list;
	
	float sum = 0.0;
	float size = 0.0;
	
	while(temp != NULL){
		
		sum += temp->cost;
		size += 1;
		
		temp = temp->next;
		
		} 
		
		return (sum/size);
	
	}
	
/*double random_gaussian(){
	
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
	
	}*/
	
/*void mutate(double beta, double pattern[], double fitness){
	
	double alpha = 0.0;
	double aux = 0.0;
	
	//static double child[SIZE];
	
	for(int i = 0; i < SIZE; i++)
		child[i] = parent[i];
				
	for(int i = 0; i < SIZE; i++){
		
		if(fitness < 0) //HERE!!!!!!
			fitness = 0;
				
		alpha = (1.0/beta) * exp(-fitness); // mutation_rate
				
		do{				
						
			aux = pattern[i] + alpha * random_gaussian();
							
		}while(aux < 0 || aux > 1); //Between 0 and 1
		
		pattern[i] = aux;
				
		}
		
	//return child;
	
	}*/
	
/*void clone_cell(double beta, int num_clones, struct Node** list, struct Node parent){ // list is the progeny list
		
	int count = 0;
		
	double * child;
	
	//delete_list(&clones);
	
	struct Node * clones = NULL; 
					
	while(count < num_clones){
				
		child = mutate(beta, parent.pattern, parent.affinity);
					
		insert_list(&clones, child);
							
		clones->cost = objective_function(child);
				
		count++;
				
		}
		
		MergeSort(&clones);
				
		insert_list(list, clones->pattern); //Inserts in progeny list -> list == progeny
		
		(*list)->cost = 0.0;
		(*list)->affinity = 0.0;
										
	}*/

	
void set_neighborhood(struct Node cell, struct Node* list, double aff_thresh){ //list receives progeny
	
	delete_list(&neighbors);
		
	struct Node* temp = list;
	
	while(temp != NULL){
		
		if(euclidian_distance(temp->pattern, cell.pattern) < aff_thresh)
			insert_list(&neighbors, temp->pattern);
		
		temp = temp->next;
		
		}
	
	}

/*void affinity_supress(struct Node* population, struct Node** cl, double aff_thresh){ // population receives progeny
			
	struct Node cell;
	
	struct Node* temp = population;
	
	delete_list(cl); *cl = NULL;
			
	while(temp != NULL){
		
		for(int i = 0; i < SIZE; i++)
			cell.pattern[i] = temp->pattern[i];
			
		cell.cost = temp->cost;
		cell.affinity = temp->affinity;
		
		set_neighborhood(cell, population, aff_thresh);
		MergeSort(&neighbors);
				
		if(neighbors == NULL || cell_equal(cell.pattern, neighbors->pattern)) 
			insert_list(cl, temp->pattern);
		
		temp = temp->next;
		
		}
		
		//printf("\n\nPop list size 2: %d\n", list_size(pop));	
		//printf("\n\nProgeny list size 2: %d\n", list_size(progeny));
	
	}*/				

/*struct Node aiNet(struct Node clone){
		
	struct Node best_d;
	
	double avgCost = 0.0;	
	
	struct Node * cl = NULL; // list for clones of clone
	
	struct Node * d = NULL; // list for best detectors
		
	insert_list(&cl, clone.pattern);
							
	for(int i = 0; i < MAX_GENS_2; i++){ //Main for
		
		evaluate(&cl);
			
		calculate_affinity(&cl);
		
		MergeSort(&cl);
		
		struct Node* temp = cl;
				
		if(d == NULL || cl->cost < d->cost){
						
			insert_list(&d, cl->pattern);
			evaluate(&d);
			
			}
				
		avgCost = avarage_cost(cl);
										                      				
	do{
			
	  delete_list(&progeny); progeny = NULL;
	  	  		
	  while(temp != NULL){ //temp == cl
				
		struct Node parent;
		
		for(int i = 0; i < SIZE; i++){ 
			
			parent.pattern[i] = temp->pattern[i];
			parent.bitstring[i] = temp->bitstring[i];
			
			}
			
		parent.cost = temp->cost;
		parent.affinity = temp->affinity;
									
		clone_cell(BETA, NUM_CLONES, &progeny, parent); // parent == cl
								
		temp = temp->next;
				
		}
			
		evaluate(&progeny);		
		temp = cl;
																
	}while(avarage_cost(progeny) >= avgCost && avgCost > 0);
							
		evaluate(&progeny);
				
		calculate_affinity(&progeny);
				
		MergeSort(&progeny);
			
		affinity_supress(progeny, &cl, AFF_THRESHOLD);
				
		random_insertion(&cl);
				
		printf("\n\ni value: %d\n", i);
		
		printf("AvgCost: %lf\n", avgCost);
        			
		} //Main for
			
	evaluate(&d);
	calculate_affinity(&d);
			
	//Set best detector values
	
	printf("\n\nBest Detector: \n\n");
	
	for(int i = 0; i < SIZE; i++)
		best_d.pattern[i] = d->pattern[i];
		
	best_d.cost = d->cost;
	best_d.affinity = d->affinity;
	
	//Printing best detector
	
	for(int i = 0; i<SIZE; i++){
		
		if(i == 0)
		
			printf("Detector: [%.2lf", best_d.pattern[i]);
					
		else if ( (i > 0) && (i < SIZE - 1))
		
			printf(", %.2lf ", best_d.pattern[i]);
						
		else
		
			printf(", %.2lf] ", best_d.pattern[i]);
			
		}
			
	//cost and affinity values
			
	printf("cost: %lf ", best_d.cost);
			
	printf("affinity: %lf ", best_d.affinity);
			
	printf("\n");
		          	
    return best_d;
	
*/

#endif

