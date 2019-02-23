/****************** CLIENT CODE ****************/

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
#include "TCPClient.h"
#include "TCPCommunication.h"

#define TRUE   1 
#define FALSE  0 
#define PORT 9005 


#define MAX_DETECTORS 300

void initialization_phase(struct Node** detectors, struct  Node** recv_memory_cells, struct Node* self_dataset, int iteration, int max_detectors){
	
	int received_antigen = list_size(*recv_memory_cells);

	printf("\nreceived antigen: %d\n", received_antigen);

	//printf("\n\n initialization RECEIVED DETECTORS: \n\n"); print_list(*recv_memory_cells);
		
	if(iteration == 0){
		
		negative_selection(detectors, self_dataset, max_detectors);
		
		}else{

                        //insert received memory cells in detectors list 

         		struct Node* temp = *recv_memory_cells;

			printf("\ntemp size: %d\n", list_size(temp));

         		while(temp != NULL){

            			insert_list(detectors, temp->pattern); 

				set_detector_radius(detectors, self_dataset);
                             
            			temp = temp->next;

         		}

			printf("\ninitialize detectors size: %d\n", list_size(*detectors));

         		delete_node_list(recv_memory_cells);
	
			printf("\ninitialize detectors size 2: %d\n", list_size(*detectors));
			
			negative_selection(detectors, self_dataset, max_detectors - received_antigen);
			
			printf("\nRANDOM DETECTORS: %d\n\n", max_detectors - received_antigen);     
			printf("\ninitialize detectors size: %d\n", list_size(*detectors));
                  			
		     }

		
		//printf("\n\n initialization DETECTORS: \n\n"); print_list(*detectors);

         	
}

void detection_phase(struct Node** detectors, struct Node* dataset){ //Negative selection
		
	struct Node * temp = *detectors;
	struct Node * temp2 = dataset;
	
	while(*detectors != NULL){

		//(*detectors)->memory_cells = NULL; //INITIALIZE MEMORY CELLS LIST
		//(*detectors)->best_detector = NULL;

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
	printf("non self antigen num: %d\n", non_self_antigen_num);		
	
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
			(*best_detectors)->radius = detectors->radius;

			//(*best_detectors)->memory_cells = NULL; //INITIALIZE MEMORY CELLS LIST
			//(*best_detectors)->best_detector = NULL;

			if((*best_detectors)->memory_cells == NULL && (*best_detectors)->best_detector == NULL)
				printf("\nMEMORY_CELLS AND BEST_DETECORS LISTS ARE NULL\n");
			
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
				
				if((*best_detectors)->counter >= counter){
					
					counter = (*best_detectors)->counter;
					temp2 = *best_detectors;
					
					}					
				
				*best_detectors = (*best_detectors)->next;
				
				}
				
				//list best_detectors with only the best detector
				
				delete_node_list(best_detectors);
				*best_detectors = temp2;	
				delete_node_list(&((*best_detectors)->next));
			
			}
		
		//select judgment agents
		
		detectors = temp;	
			
		int n = list_size(detectors)/2;
				
		int i = 0;
				
		while(i < n){
		
			while(detectors != NULL){

				int c = 0;

                                if(max_counter > 0)
					c = rand()%max_counter;
			
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

	/*printf("\n\nJudgment agents: \n\n");
  
  	print_list(*judgment_agents);*/
				
	while(*judgment_agents != NULL){ //Mutating -> Moving near best agent
										
		//mutating one clone instance in clones each time function is called
			
		hypermutate(judgment_agents);
								
		*judgment_agents = (*judgment_agents)->next;
				
		} 
			
	*judgment_agents = temp;

	//Sorting by fitness and fixing agents control population
	
	sort_fitness(detectors);
	
	/*int size = list_size(*judgment_agents);
	
	temp = *detectors;
	
	struct Node* next = NULL;
	
	for(int i = 0; i<size; i++){
		
		next = temp->next;
		free(temp);
		temp = next;
		
		} 
		
		*detectors = temp;*/

         //insert jugdment clones in detectors list
 
         /*temp = *judgment_agents;

         while(temp != NULL){

            insert_list(detectors, temp->pattern);

            temp = temp->next;
       
         }*/
		
	}

void judgment_phase(struct Node** judgment_clones, struct Node** memory_cells, struct Node* self_dataset){ //Immune Network
	
	//set judgment_clones radius
	
	struct Node* temp = *judgment_clones;
	
	while(*judgment_clones != NULL){
		
		set_detector_radius(judgment_clones, self_dataset);		
		*judgment_clones = (*judgment_clones)->next;
		
		}
		
	*judgment_clones = temp;
		
	struct Node* temp2 = *memory_cells;
	
	while(*judgment_clones != NULL){
		
		while(*memory_cells != NULL){
			
			if(euclidian_distance((*judgment_clones)->pattern, (*memory_cells)->pattern) < (*judgment_clones)->radius)
				(*memory_cells)->confirm += 1;
			
			*memory_cells = (*memory_cells)->next;
			
			}
			
		*memory_cells = temp2;	
		*judgment_clones = (*judgment_clones)->next;	
		
		}
		
	*judgment_clones = temp;
	*memory_cells = temp2;
	
	sort_confirm(memory_cells);
	
	}


int main(){

  srand ( time(NULL) );

  struct Node* detectors = NULL;
  struct Node* self_dataset = NULL; 
  struct Node* train_dataset = NULL; 
  struct Node* best_detector = NULL;
  struct Node* judgment_agents = NULL;

  struct Node* recv_memory_cells = NULL; 

  struct Node* final_detectors = NULL;

  double max_fitness = 0.0;

  const int max_detectors = 150; //Number of cells to send or receive
  
  //generate datasets...
   
  generate_dataset(&self_dataset, "Self_Dataset.txt");
  
  generate_dataset(&train_dataset, "Dataset.txt");

  start_connection();

  double accuracy = -1;

  for(int iteration = 0; iteration < ITERATIONS; iteration++){
  

	printf("\niteration: %d\n", iteration);
        
	//delete lists
	
	if(iteration != 0){

		printf("\nBEST DETECTOR print\n");

		print_list(best_detector->memory_cells);

        	delete_node_list(&detectors);

        	delete_node_list(&judgment_agents);

        	delete_node_list(&best_detector);
	}

        //--- deleted lists
	
	printf("\n\ndetectors list size: %d\n", list_size(detectors));
    
  	initialization_phase(&detectors, &recv_memory_cells, self_dataset, iteration, max_detectors);

	printf("\n\ndetectors list size 2: %d\n", list_size(detectors));
  
  	detection_phase(&detectors, train_dataset);

	printf("\n\ndetectors list size 3: %d\n", list_size(detectors));

  	calculation_phase(&detectors, "Dataset.txt");

	printf("\n\ndetectors list size 4: %d\n", list_size(detectors));

	/*printf("\n\nGenerated detectors: \n\n"); 

	sort_fitness(&detectors);

	print_list(detectors);
  
  	printf("\n\n");*/

	printf("\n\ndetectors list size 5: %d\n", list_size(detectors));

  	selection_phase(detectors, &best_detector, &judgment_agents);	

	printf("\nBEST DETECTOR print 2\n");

	print_list(best_detector->memory_cells);

	printf("\n\ndetectors list size 6: %d\n", list_size(detectors));
    
  	printf("\n");
  
  	//printf("Judgment agents: \n\n");
  
  	//print_list(judgment_agents);

        printf("\n\ndetectors list size 7: %d\n", list_size(detectors));

	printf("\nACCURACY: %.2lf%%\n", get_detectors_accuracy(detectors, train_dataset));

	printf("\n\ndetectors list size 8: %d\n", list_size(detectors));

	if(get_detectors_accuracy(detectors, train_dataset) > get_detectors_accuracy(final_detectors, train_dataset)){

		delete_node_list(&final_detectors);

		struct Node* temp = detectors;
		
		while(temp != NULL){

			insert_list(&final_detectors, temp->pattern);

			final_detectors->fitness = temp->fitness;
			final_detectors->counter = temp->counter;
			final_detectors->correct = temp->correct;
			final_detectors->radius = temp->radius;
		
			temp = temp->next;			
		}

	}

	printf("\n\ndetectors list size 9: %d\n", list_size(detectors));

  	detection_phase_2(&judgment_agents, &detectors, best_detector);

	printf("\nBEST DETECTOR print 3\n");

	print_list(best_detector->memory_cells);

	printf("\n\ndetectors list size 10: %d\n", list_size(detectors));
  
  	//printf("\n\ndetectors:\n\n");
  
  	//print_list(detectors);

	printf("\n\ndetectors list size 11: %d\n", list_size(detectors));
  
  	judgment_phase(&judgment_agents, &best_detector->memory_cells, self_dataset);

	printf("\nBEST DETECTOR print 4\n");

	print_list(best_detector->memory_cells);

	printf("\n\ndetectors list size 12: %d\n", list_size(detectors));

	/*printf("\nBEST DETECTOR: \n\n");
	print_list(best_detector);*/

	/*printf("\n\nCloned Judgment agents: \n\n");  
  	print_list(judgment_agents);*/


	/*printf("\n\nBEST DETECTOR'S MEMORY CELLS:\n\n");
	print_list(best_detector->memory_cells);*/

  	client(best_detector->memory_cells, &recv_memory_cells, max_detectors);

	printf("\nBEST DETECTOR print 5\n");

	print_list(best_detector->memory_cells);

  	printf("\n\ndetectors list size 13: %d\n", list_size(detectors));
	
  
  }

  close_client();

  sort_fitness(&final_detectors);

  printf("\nFINAL DETECTORS\n\n");
  print_list(final_detectors);

  print_detectors_performance(final_detectors, train_dataset);

  return 0;
}
