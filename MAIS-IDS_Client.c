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
#include "graph.h"
#include "acg.h"

graph_node* sag = NULL;
graph_node* dag = NULL; 
alert_path* acg = NULL; 

//MAIS-IDS performance
metric_data ids_acc; double ids_acc_data[SYSTEM_RUNS];
metric_data ids_fp; double ids_fp_data[SYSTEM_RUNS];
metric_data ids_fn; double ids_fn_data[SYSTEM_RUNS];
metric_data ids_dr; double ids_dr_data[SYSTEM_RUNS];

//MAIS-IDS with ACG performance
metric_data acg_acc; double acg_acc_data[SYSTEM_RUNS];
metric_data acg_fp; double acg_fp_data[SYSTEM_RUNS];
metric_data acg_fn; double acg_fn_data[SYSTEM_RUNS];
metric_data acg_dr; double acg_dr_data[SYSTEM_RUNS];

int system_runs = 0;

void initialization_phase(struct Node** detectors, struct  Node** recv_memory_cells, struct Node* self_dataset, int iteration, int max_detectors){
		
	if(iteration == 0){
		
		generate_detectors(detectors, self_dataset, max_detectors);
		
		}else{

                        //insert received memory cells in detectors list 

         		struct Node* temp = *recv_memory_cells;

         		while(temp != NULL){

				if(!euclidian_distance_match_list(*detectors, temp->pattern, 0.0)){

            				insert_list(detectors, temp->pattern); 
					set_detector_radius(detectors, self_dataset);
				}
                             
            			temp = temp->next;

         		}

         		delete_node_list(recv_memory_cells);

			int received_antigen = list_size(*detectors);
	
			generate_detectors(detectors, self_dataset, max_detectors - received_antigen);
			             			
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
				
                if(strncmp(lastAttribute,"normal",6) == 0)      
			self_antigen_num++;
		else
			non_self_antigen_num++;
					
			}
			
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
							
			struct Node* pointer_aux_best = NULL; // points to best detector
			struct Node* pointer_aux_first = *best_detectors; // points to first element in best detectors list

			while(*best_detectors != NULL){
				
				if((*best_detectors)->counter >= counter){
					
					counter = (*best_detectors)->counter;
					pointer_aux_best = *best_detectors;
					
					}					
				
				*best_detectors = (*best_detectors)->next;
				
				}
				
				//list best_detectors with only the best detector

				*best_detectors = NULL;

				insert_list(best_detectors, pointer_aux_best->pattern);

				while(pointer_aux_best->memory_cells != NULL){

					insert_memorycell(best_detectors, pointer_aux_best->memory_cells->pattern);
					
					pointer_aux_best->memory_cells = pointer_aux_best->memory_cells->next;

				}
				
				delete_node_list(&pointer_aux_first); //delete all detectors from best detectors's list
							
			}
		
		detectors = temp;	
			
		int n = list_size(detectors)/2;
				
		int i = 0;

		//select judgment agents
				
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
			
	while(*judgment_agents != NULL){ //Mutating -> Moving near best agent
										
		//mutating one clone instance in clones each time function is called
			
		hypermutate(judgment_agents);
								
		*judgment_agents = (*judgment_agents)->next;
				
		} 
			
	*judgment_agents = temp;

	//Sorting by fitness and fixing agents control population
	
	sort_fitness(detectors);
		
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

	if(correct > 0){

		ids_acc_data[system_runs] = (double)correct/((double)correct+(double)false_positive+(double)false_negative)*100;

		//set acc max and min values

		if(ids_acc.max < ids_acc_data[system_runs]) ids_acc.max = ids_acc_data[system_runs];

		if(ids_acc.min > ids_acc_data[system_runs] || ids_acc.min == -1) ids_acc.min = ids_acc_data[system_runs];

		printf("\nAccuracy: %.2lf%%\n", ((double)correct/((double)correct+(double)false_positive+(double)false_negative))*100);
	}

	else{
		ids_acc.min = 0.0;
		printf("\nAccuracy 0.0%%\n");
	}

	if(false_positive > 0){

		ids_fp_data[system_runs] = (((double)false_positive/(double)(true_negative+false_positive))*100);

		//set fp max and min values

		if(ids_fp.max < ids_fp_data[system_runs]) ids_fp.max = ids_fp_data[system_runs];

		if(ids_fp.min > ids_fp_data[system_runs] || ids_fp.min == -1) ids_fp.min = ids_fp_data[system_runs];

		printf("False postive: %.2lf%%\n", (((double)false_positive/(double)(true_negative+false_positive))*100));
	
	}
	else{
		ids_fp.min = 0.0;
		printf("False positive: 0.0%%\n");

	}

	if(false_negative > 0){

		ids_fn_data[system_runs] = (((double)false_negative/(double)(true_positive+false_negative))*100);

		//set fn max and min values

		if(ids_fn.max < ids_fn_data[system_runs]) ids_fn.max = ids_fn_data[system_runs];

		if(ids_fn.min > ids_fn_data[system_runs] || ids_fn.min == -1) ids_fn.min = ids_fn_data[system_runs];
		
		printf("False negative: %.2lf%%\n", (((double)false_negative/(double)(true_positive+false_negative))*100));

	}
	else{
		ids_fn.min = 0.0;
		printf("False negative: 0.0%%\n");

	}

	if(true_positive > 0){

		ids_dr_data[system_runs] =  (((double)true_positive/(double)(true_positive+false_negative))*100);

		//set dr max and min values

		if(ids_dr.max < ids_dr_data[system_runs]) ids_dr.max = ids_dr_data[system_runs];

		if(ids_dr.min > ids_dr_data[system_runs] || ids_dr.min == -1) ids_dr.min = ids_dr_data[system_runs];
		
		printf("Detection rate: %.2lf%%\n", (((double)true_positive/(double)(true_positive+false_negative))*100));
	}
	else{
		ids_dr.min = 0.0;
		printf("Detection rate: 0.0%%\n");
	}


}

void print_acg_performance(struct Node* detectors, struct Node* dataset){

	struct Node* temp = dataset;
	
	int true_positive = 0;
        int true_negative = 0;
	int self_antigen_num = 0;
	int non_self_antigen_num = 0;

	int false_positive = 0;
	int false_negative = 0;
	
	alert a; 

	while(temp != NULL){

		if(temp->instance_class)
			non_self_antigen_num++;
		else
			self_antigen_num++;

		temp = temp->next;

	}

	int total = non_self_antigen_num + self_antigen_num;

	while(dataset != NULL){

		strcpy(a.src_hostname, dataset->src_ip);
		strcpy(a.dst_hostname, dataset->dst_ip);
		a.seconds_elapsed = dataset->seconds_elapsed; 

		//printf("seconds: %lf ", dataset->seconds_elapsed); printf("seconds: %lf\n\n ", a.seconds_elapsed);

		if(euclidian_distance_match_radius(detectors, dataset->pattern) && alert_correlation(&dag, &acg, a)){

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

		memset(a.src_hostname, 0, strlen(a.src_hostname)*sizeof(char));	
		memset(a.dst_hostname, 0, strlen(a.dst_hostname)*sizeof(char));

		initialize_graph(&dag); //reset path

		//print_acg(acg);
		
		dataset = dataset->next;

	}

	int correct = true_positive+true_negative;

	if(correct > 0){

		acg_acc_data[system_runs] = (double)correct/((double)correct+(double)false_positive+(double)false_negative)*100;

		//set acc max and min values

		if(acg_acc.max < acg_acc_data[system_runs]) acg_acc.max = acg_acc_data[system_runs];

		if(acg_acc.min > acg_acc_data[system_runs] || acg_acc.min == -1) acg_acc.min = acg_acc_data[system_runs];

		printf("\nAccuracy: %.2lf%%\n", ((double)correct/((double)correct+(double)false_positive+(double)false_negative))*100);
	}

	else{
		acg_acc.min = 0.0;
		printf("\nAccuracy 0.0%%\n");
	}

	if(false_positive > 0){

		acg_fp_data[system_runs] = (((double)false_positive/(double)(true_negative+false_positive))*100);

		//set fp max and min values

		if(acg_fp.max < acg_fp_data[system_runs]) acg_fp.max = acg_fp_data[system_runs];

		if(acg_fp.min > acg_fp_data[system_runs] || acg_fp.min == -1) acg_fp.min = acg_fp_data[system_runs];

		printf("False postive: %.2lf%%\n", (((double)false_positive/(double)(true_negative+false_positive))*100));
	
	}
	else{
		acg_fp.min = 0.0;
		printf("False positive: 0.0%%\n");

	}

	if(false_negative > 0){

		acg_fn_data[system_runs] = (((double)false_negative/(double)(true_positive+false_negative))*100);

		//set fn max and min values

		if(acg_fn.max < acg_fn_data[system_runs]) acg_fn.max = acg_fn_data[system_runs];

		if(acg_fn.min > acg_fn_data[system_runs] || acg_fn.min == -1) acg_fn.min = acg_fn_data[system_runs];
		
		printf("False negative: %.2lf%%\n", (((double)false_negative/(double)(true_positive+false_negative))*100));

	}
	else{
		acg_fn.min = 0.0;
		printf("False negative: 0.0%%\n");

	}

	if(true_positive > 0){

		acg_dr_data[system_runs] =  (((double)true_positive/(double)(true_positive+false_negative))*100);

		//set dr max and min values

		if(acg_dr.max < acg_dr_data[system_runs]) acg_dr.max = acg_dr_data[system_runs];

		if(acg_dr.min > acg_dr_data[system_runs] || acg_dr.min == -1) acg_dr.min = acg_dr_data[system_runs];
		
		printf("Detection rate: %.2lf%%\n", (((double)true_positive/(double)(true_positive+false_negative))*100));
	}
	else{
		acg_dr.min = 0.0;
		printf("Detection rate: 0.0%%\n");
	}

}
				

int main(int argc, char *argv[]){

  srand ( time(NULL) );

  struct Node* detectors = NULL;
  struct Node* self_dataset = NULL; 
  struct Node* train_dataset = NULL; 
  struct Node* test_dataset = NULL;
  struct Node* best_detector = NULL;
  struct Node* judgment_agents = NULL;

  struct Node* recv_memory_cells = NULL; 

  struct Node* final_detectors = NULL;

  double max_fitness = 0.0;

  if(argc < 3){

  	printf("Argument(s) expected.\n");
  	return 0;

  	}
  
   else if(argc > 3){
 
  	printf("Too many arguments supplied.\n");
	return 0;	

	} 

  const int max_detectors = atoi(argv[1]); //Number of cells to send or receive
  const int vm_number = atoi(argv[2]);;

  ids_acc.min = -1; ids_fp.min = -1; ids_fn.min = -1; ids_dr.min = -1;
  acg_acc.min = -1; acg_fp.min = -1; acg_fn.min = -1; acg_dr.min = -1;

  initialize_pattern_attr_range();

  //set datassets ranges

  set_pattern_attr_range("normal.txt");
  set_pattern_attr_range("train.txt");
  set_pattern_attr_range("test.txt");

  //generate datasets...
   
  generate_dataset(&self_dataset, "normal.txt", "null"); //self dataset

  generate_dataset(&train_dataset, "train.txt", "null");

  generate_dataset(&test_dataset, "test.txt", "alert_data.txt");

  sort_dataset(&test_dataset);

  inicialization(&sag, "AttackGraph.txt"); //generate sag

  convert_to_dag(&dag, &sag);	

  for(int i = 0; i < SYSTEM_RUNS; i++){ // number of times the system runs

  	start_connection();

  	double accuracy = -1;

	//DELETE ALL LISTS

  	delete_node_list(&detectors);
	delete_node_list(&best_detector);
	delete_node_list(&judgment_agents);
	delete_node_list(&recv_memory_cells);
	delete_node_list(&final_detectors);

	delete_acg(&acg); //delete alert correlation graph

  	for(int iteration = 0; iteration < ITERATIONS; iteration++){
  
		//delete lists
	
		if(iteration != 0){

        		delete_node_list(&detectors);

        		delete_node_list(&judgment_agents);

        		delete_node_list(&best_detector);
		}

        	//--- deleted lists

                //printf("INITIALIZATION PHASE \n\n");
	
  		initialization_phase(&detectors, &recv_memory_cells, self_dataset, iteration, max_detectors);

  		//printf("DETECTION PHASE \n\n");

  		detection_phase(&detectors, train_dataset);

		//printf("CALCULATION PHASE \n\n");

  		calculation_phase(&detectors, "train.txt");

 		//printf("SELECTION PHASE \n\n");

  		selection_phase(detectors, &best_detector, &judgment_agents);

                //printf("SET FINAL DETECTORS \n\n");	

		//gets detectors's set with higher accuracy
  
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

                //printf("DETECTION PHASE 2\n\n");

  		detection_phase_2(&judgment_agents, &detectors, best_detector);

                //printf("JUDGMENT PHASE \n\n");

  		judgment_phase(&judgment_agents, &best_detector->memory_cells, self_dataset);

                //printf("CLIENT PHASE \n\n");

  		client(best_detector->memory_cells, &recv_memory_cells, max_detectors, vm_number);

  	}

        printf("\n\n");

  	sort_fitness(&final_detectors);

  	printf("WITHOUT ACG\n\n");

  	print_detectors_performance(final_detectors, test_dataset);

  	printf("\n\nWITH ACG\n\n");

  	print_acg_performance(final_detectors, test_dataset);

  	close_client(); 

	sleep(2);

	system_runs++;	

  }

  //calulate mean

  for(int i = 0; i < SYSTEM_RUNS; i++){

	ids_acc.mean += ids_acc_data[i]; ids_fp.mean += ids_fp_data[i]; ids_fn.mean += ids_fn_data[i]; ids_dr.mean += ids_dr_data[i];
	acg_acc.mean += acg_acc_data[i]; acg_fp.mean += acg_fp_data[i]; acg_fn.mean += acg_fn_data[i]; acg_dr.mean += acg_dr_data[i];

  }

  //IDS mean

  ids_acc.mean = (double) ids_acc.mean/(double) SYSTEM_RUNS; ids_fp.mean = (double) ids_fp.mean/(double) SYSTEM_RUNS; 
  ids_fn.mean = (double) ids_fn.mean/(double) SYSTEM_RUNS; ids_dr.mean = (double) ids_dr.mean/(double) SYSTEM_RUNS;

  //ACG mean

  acg_acc.mean = (double) acg_acc.mean/(double) SYSTEM_RUNS; acg_fp.mean = (double) acg_fp.mean/(double) SYSTEM_RUNS; 
  acg_fn.mean = (double) acg_fn.mean/(double) SYSTEM_RUNS; acg_dr.mean = (double) acg_dr.mean/(double) SYSTEM_RUNS;

  //calculate variance

  for(int i = 0; i < SYSTEM_RUNS; i++){

  	ids_acc.var += pow((ids_acc_data[i] - ids_acc.mean), 2); ids_fp.var += pow((ids_fp_data[i] - ids_fp.mean), 2);
  	ids_fn.var +=pow((ids_fn_data[i] - ids_fn.mean), 2); ids_dr.var += pow((ids_dr_data[i] - ids_dr.mean), 2);

	acg_acc.var += pow((acg_acc_data[i] - acg_acc.mean), 2); acg_fp.var += pow((acg_fp_data[i] - acg_fp.mean), 2);
  	acg_fn.var += pow((acg_fn_data[i] - acg_fn.mean), 2); acg_dr.var += pow((acg_dr_data[i] - acg_dr.mean), 2);

  }

  //IDS variance

  ids_acc.var = (double) ids_acc.var/(double) SYSTEM_RUNS; ids_fp.var = (double) ids_fp.var/(double) SYSTEM_RUNS; 
  ids_fn.var = (double) ids_fn.var/(double) SYSTEM_RUNS; ids_dr.var = (double) ids_dr.var/(double) SYSTEM_RUNS;

  //ACG variance

  acg_acc.var = (double) acg_acc.var/(double) SYSTEM_RUNS; acg_fp.var = (double) acg_fp.var/(double) SYSTEM_RUNS; 
  acg_fn.var = (double) acg_fn.var/(double) SYSTEM_RUNS; acg_dr.var = (double) acg_dr.var/(double) SYSTEM_RUNS;

  //calculate standard deviation

  ids_acc.std = sqrt(ids_acc.var); ids_fp.std = sqrt(ids_fp.var); ids_fn.std = sqrt(ids_fn.var); ids_dr.std = sqrt(ids_dr.var);
  acg_acc.std = sqrt(acg_acc.var); acg_fp.std = sqrt(acg_fp.var); acg_fn.std = sqrt(acg_fn.var); acg_dr.std = sqrt(acg_dr.var);

  //print performance -> mean, fp, fn, dr

  printf("\n");

  printf("PERFORMANCE WITHOUT ACG\n\n");

  printf("acc mean: %.3lf", ids_acc.mean); printf(" fp mean: %.3lf ", ids_fp.mean); 
  printf("fn mean: %.3lf", ids_fn.mean); printf(" dr mean: %.3lf ", ids_dr.mean);
  printf("\n\n");

  printf("acc std: %.3lf", ids_acc.std); printf(" fp std: %.3lf ", ids_fp.std); 
  printf("fn std: %.3lf", ids_fn.std); printf(" dr std: %.3lf ", ids_dr.std);
  printf("\n\n");

  printf("acc max: %.3lf", ids_acc.max); printf(" fp max: %.3lf ", ids_fp.max); 
  printf("fn max: %.3lf", ids_fn.max); printf(" dr max: %.3lf ", ids_dr.max);
  printf("\n\n");

  printf("acc min: %.3lf", ids_acc.min); printf(" fp min: %.3lf ", ids_fp.min); 
  printf("fn min: %.3lf", ids_fn.min); printf(" dr min: %.3lf ", ids_dr.min);
  printf("\n\n");

  printf("PERFORMANCE WITH ACG\n\n");

  printf("acc mean: %.3lf", acg_acc.mean); printf(" fp mean: %.3lf ", acg_fp.mean); 
  printf("fn mean: %.3lf", acg_fn.mean); printf(" dr mean: %.3lf ", acg_dr.mean);  
  printf("\n\n");

  printf("acc std: %.3lf", acg_acc.std); printf(" fp std: %.3lf ", acg_fp.std); 
  printf("fn std: %.3lf", acg_fn.std); printf(" dr std: %.3lf ", acg_dr.std);
  printf("\n\n");

  printf("acc max: %.3lf", acg_acc.max); printf(" fp max: %.3lf ", acg_fp.max); 
  printf("fn max: %.3lf", acg_fn.max); printf(" dr max: %.3lf ", acg_dr.max);
  printf("\n\n");

  printf("acc min: %.3lf", acg_acc.min); printf(" fp min: %.3lf ", acg_fp.min); 
  printf("fn min: %.3lf", acg_fn.min); printf(" dr min: %.3lf ", acg_dr.min);
  printf("\n\n");

  return 0;
}
