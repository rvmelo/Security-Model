#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h> 

#include "NSA_NSL_KDD.h"
#include "CLONALG_NSL_KDD.h"
#include "aiNet_NSL_KDD.h"
#include "SortingLists.h"
#include "SocketTCP.h"

#define TRUE   1 
#define FALSE  0 
#define PORT 9005 


#define MAX_DETECTORS 300

void initialization_phase(struct Node** detectors, struct Node* self_dataset, int iteration, int max_detectors){
	
	int received_antigen = 10;
		
	if(iteration == 0){
		
		negative_selection(detectors, self_dataset, max_detectors);
		
		}else{
			
			negative_selection(detectors, self_dataset, list_size(*detectors) - received_antigen);
			
			}
	
	}

void detection_phase(struct Node** detectors, struct Node* dataset){ //Negative selection
		
	struct Node * temp = *detectors;
	struct Node * temp2 = dataset;
	
	while(*detectors != NULL){
		
		while(dataset != NULL){
			
			if(euclidian_distance(dataset->pattern, (*detectors)->pattern) < (*detectors)->radius){
				
				(*detectors)->counter += 1; 
				insert_memorycell(detectors, dataset->pattern);
				
				if(dataset->instance_class)
					(*detectors)->correct += 1;
				
				}
				
				dataset = dataset->next;
			
			}
			
			*detectors = (*detectors)->next;
			dataset = temp2;
		
		}
	
	 *detectors = temp;
	
	}		

void calculation_phase(struct Node** detectors, char* file_name){
	
	int lines = 0;
	int lineCount = 0;
	
	int self_antigen_num = 0;
	int non_self_antigen_num = 0;
	
	char * lastAttribute;
	
	FILE * f_Calculation_Phase_Pointer = fopen(file_name, "r");
	
	lineCount = GetFileLines(file_name);
		
	struct Node* temp = *detectors;
	
	// get self_antigen_num and non_self_antigen_num values
		
	while(lines < lineCount){
				
		lastAttribute = GetInstanceClass(&f_Calculation_Phase_Pointer);
                				
		lines++;
				
		//if((int)strlen(lastAttribute) == 7 || (int)strlen(lastAttribute) == 8)   // lastAttribute == normal
                if(strncmp(lastAttribute,"normal",6) == 0)      
			self_antigen_num++;
		else
			non_self_antigen_num++;
					
			}
			
	printf("\nself antigen num: %d\n", self_antigen_num);
	printf("non self antigen num: %d\n\n", non_self_antigen_num);		
	
	// calculate each detector's fitness		
	
	while(*detectors != NULL){
								
		if(non_self_antigen_num > 0)
			(*detectors)->fitness = (double)(*detectors)->correct/(double)non_self_antigen_num; 
		else
			(*detectors)->fitness = 0.0;
		
		*detectors = (*detectors)->next;
				
		}
				
		*detectors = temp;
		
		fclose(f_Calculation_Phase_Pointer);
	
	}
	
void selection_phase(struct Node* detectors, struct Node** best_detectors, struct Node** judgment_agents){
	
	struct Node* temp = detectors;
			
	double max_fitness = 0.0;
	int counter = 0;
	
	int max_counter = 0; // to select judgment_agents
	
	// find max fitness value
	
	while(detectors != NULL){
		
		if(detectors->fitness > max_fitness)
			max_fitness = detectors->fitness;
			
		if(max_counter < detectors->counter)
				max_counter = detectors->counter;
			
		detectors = detectors->next;
		
		}
		
		detectors = temp;
	
	//find best detectors with the highest fitness	
		
	while(detectors != NULL){
		
		if(detectors->fitness == max_fitness){
			
			insert_list(best_detectors, detectors->pattern);
			(*best_detectors)->counter = detectors->counter;
			(*best_detectors)->fitness = detectors->fitness;
			
			struct Node* temp2 = detectors->memory_cells;
			
			while(detectors->memory_cells != NULL){
					
				insert_memorycell(best_detectors, detectors->memory_cells->pattern);
				
				detectors->memory_cells = detectors->memory_cells->next;
				
				}
				
				detectors->memory_cells = temp2;
			
			}
			
			detectors = detectors->next;
		
		}
		
		//find best detector 
		
		if(list_size(*best_detectors) > 1){
			
			struct Node* temp2 = NULL;
			
			while(*best_detectors != NULL){
				
				if((*best_detectors)->counter > counter){
					
					counter = (*best_detectors)->counter;
					temp2 = *best_detectors;
					
					}
				
				*best_detectors = (*best_detectors)->next;
				
				}
				
				//list best_detectors with only the best detector
				
				delete_list(best_detectors);
				*best_detectors = temp2;
				(*best_detectors)->next = NULL;
			
			}
		
		//select judgment agents
		
		detectors = temp;	
			
		int n = list_size(detectors)/2;
				
		int i = 0;
				
		while(i < n){
		
			while(detectors != NULL){
		
				int c = rand()%max_counter;
			
				if(detectors->counter >= c && !euclidian_distance_match_list(*judgment_agents, detectors->pattern, 0.0)
				&& !cell_equal((*best_detectors)->pattern, detectors->pattern)){
				
					insert_list(judgment_agents, detectors->pattern);
					i++;
				
					}
				
				detectors = detectors->next;
						
			}
			
			detectors = temp;			
				
		}				
	}

void detection_phase_2(struct Node** judgment_agents, struct Node** detectors, struct Node* best_detector){ //clonal selection
	
	struct Node* temp = *judgment_agents;
	
	while(*judgment_agents != NULL){ //informed by best detector
		
		insert_best_detector(judgment_agents, best_detector->pattern);				
		*judgment_agents = (*judgment_agents)->next;
		
		}
	
	*judgment_agents = temp; 
				
	while(*judgment_agents != NULL){ //Mutating -> Moving near best agent
												
		//mutating one clone instance in clones each time function is called
					
		hypermutate(judgment_agents);
										
		*judgment_agents = (*judgment_agents)->next;
				
		} 
			
	*judgment_agents = temp;
	
	//Sorting by fitness and fixing agents control population
	
	sort_fitness(detectors);
	
	int size = list_size(*judgment_agents);
	
	temp = *detectors;
	
	struct Node* next = NULL;
	
	for(int i = 0; i<size; i++){
		
		next = temp->next;
		free(temp);
		temp = next;
		
		} 
		
		*detectors = temp;		
		
	}

void judgment_phase(struct Node** judgment_clones, struct Node* memory_cells, struct Node* self_dataset){ //Immune Network
	
	//set judgment_clones radius
	
	struct Node* temp = *judgment_clones;
	
	while(*judgment_clones != NULL){
		
		set_detector_radius(judgment_clones, self_dataset);		
		*judgment_clones = (*judgment_clones)->next;
		
		}
		
	*judgment_clones = temp;
		
	struct Node* temp2 = memory_cells;
	
	while(*judgment_clones != NULL){
		
		while(memory_cells != NULL){
			
			if(euclidian_distance((*judgment_clones)->pattern, memory_cells->pattern) < (*judgment_clones)->radius)
				memory_cells->confirm += 1;
			
			memory_cells = memory_cells->next;
			
			}
			
		memory_cells = temp2;	
		*judgment_clones = (*judgment_clones)->next;	
		
		}
		
	*judgment_clones = temp;
	memory_cells = temp2;
	
	sort_confirm(&memory_cells);
	
	printf("\n\n"); printf("Memory cells to send:\n\n");

        int i =0;	
		
	while(memory_cells != NULL){
		
		for(int i = 0; i < SIZE; i++)
			printf("%.2lf ", memory_cells->pattern[i]);
		
		printf("confirm %d: %d ", i+1, memory_cells->confirm);	
		printf("\n");

                i++;   
		
		memory_cells = memory_cells->next;
		
		}
	
	//Send to orchestra...	
	
	}

void orchestra_old(struct Node** destiny_memory_cells, struct Node* origin_memory_cells){
	
	//receive memory cells from each virtual machine
	
	//removing extra antigens from destiny_memory_cells
	
	int size = list_size(*destiny_memory_cells);
	
	struct Node* temp = *destiny_memory_cells;
	
	struct Node* next = NULL;
	
	for(int i = 0; i<size-5; i++){
		
		next = temp->next;
		free(temp);
		temp = next;
		
		} 
		
	*destiny_memory_cells = temp;
	
	//removing extra antigens from origin_memory_cells
		
	size = list_size(origin_memory_cells);
	
	temp = origin_memory_cells;
	
	next = NULL;
	
	for(int i = 0; i<size-5; i++){
		
		next = temp->next;
		free(temp);
		temp = next;
		
		} 
	
	origin_memory_cells = temp;
	
	//merging memory cells of each two virtual machines	 		
	
	while(origin_memory_cells != NULL){
		
		insert_list(destiny_memory_cells, origin_memory_cells->pattern);
		
		origin_memory_cells = origin_memory_cells->next;
		
		}
		 		
    //Send to destination virtual machine
		 		
	}

/*int writen(const int sd, const char * b, const size_t s, const int retry_on_interrupt)
{
  size_t n = s;
  while (0 < n)
  {
    ssize_t result = write(sd, b, n);
    if (-1 == result)
    {
      if ((retry_on_interrupt && (errno == EINTR)) || (errno == EWOULDBLOCK) || (errno == EAGAIN))
      {
        continue;
      }
      else
      {
        break;
      }
    }

    n -= result;
    b += result;
  }

  return (0 < n) ?-1 :0;
}*/					

int main()
{

  srand ( time(NULL) );

  start_connection();

  check_incoming_connections(2); //number of vms

  orchestra(2);
  
  
  return 0;
}
