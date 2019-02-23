#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
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
#include "TCPServer.h"
#include "TCPCommunication.h"

#include "graph.h"
#include "acg.h"

#define MAX 44

int list[MAX];

int number = 0;

cm_metric* benefit = NULL;
cm_metric* ROI = NULL;

//SDN network countermeasures

cm_data cm[] = {
	
	{.id = 1, .name = "Traffic redirection", .condition = TRUE, .cost = 3.0f, .intrusiveness = 3.0f, .effectiveness = 0.85f},
	{.id = 2, .name = "Traffic isolation", .condition = TRUE, .cost = 2.0f, .intrusiveness = 4.0f, .effectiveness = 0.85f},
	{.id = 3, .name = "Deep Packet Inspection", .condition = TRUE, .cost = 3.0f, .intrusiveness = 3.0f, .effectiveness = 0.85f},
	{.id = 4, .name = "Creating filtering rules", .condition = TRUE, .cost = 2.0f, .intrusiveness = 1.0f, .effectiveness = 0.85f},
	{.id = 5, .name = "MAC address change", .condition = TRUE, .cost = 1.0f, .intrusiveness = 2.0f, .effectiveness = 0.85f},
	{.id = 6, .name = "IP address change", .condition = TRUE, .cost = 1.0f, .intrusiveness = 2.0f, .effectiveness = 0.85f},
	{.id = 7, .name = "Block port", .condition = TRUE, .cost = 1.0f, .intrusiveness = 4.0f, .effectiveness = 0.85f},
	{.id = 8, .name = "Software patch", .condition = TRUE, .cost = 4.0f, .intrusiveness = 5.0f, .effectiveness = 0.85f},
	{.id = 9, .name = "Quarantine", .condition = TRUE, .cost = 2.0f, .intrusiveness = 5.0f, .effectiveness = 0.85f},
	{.id = 10, .name = "Network reconfiguration", .condition = TRUE, .cost = 5.0f, .intrusiveness = 0.0f, .effectiveness = 0.85f},
	{.id = 11, .name = "Network topology change", .condition = TRUE, .cost = 5.0f, .intrusiveness = 0.0f, .effectiveness = 0.85f}

};

void insert_cm_metric(cm_metric** list, cm_metric data){

	cm_metric* temp = (cm_metric*)malloc(sizeof(cm_metric));

	temp->node_id = data.node_id;
	temp->cm_id = data.cm_id;
	temp->value = data.value;

	temp->next = *list;
	*list = temp;
}

void delete_cm_metric(cm_metric** head_ref){

	/* deref head_ref to get the real head */
	cm_metric* current = *head_ref;
   	cm_metric* next;

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

bool calculate_pr_bool(graph_edge* predecessors){ //if node probability can be calculated

	while(predecessors != NULL){

		if(predecessors->node->data.pr_set == false) return false;

		predecessors = predecessors->next;

	}

	return true;

}

void enable_node(graph_node** graph){ //enable node to calculate pr_T

	/*graph_edge* edge_aux = sag_aux->edge;

	while(edge_aux != NULL){ // enable sucessors to be calculated by setting their predecessors pr_set to "true"  

		graph_node* node_aux = edge_aux->node;

		set_predecessors_pr_set(&(node_aux)->predecessor_edge);

		edge_aux = edge_aux->next;

	}*/

	graph_edge* edge_aux = (*graph)->predecessor_edge;

	while(edge_aux != NULL){ // enable predecessors to be calculated by setting their predecessors pr_set to "true" 

		graph_node* node_aux = edge_aux->node;

		set_predecessors_pr_set(&(node_aux)->predecessor_edge);

		edge_aux = edge_aux->next;

	}

	(*graph)->data.pr_set = false;

}

void enable_graph_nodes(graph_node** graph){

	if((*graph) == NULL){
		
		return;
	}
	
	else{

		enable_node(graph);
		
		graph_edge* temp = (*graph)->edge;

		while((*graph)->edge != NULL){
	
			enable_graph_nodes(&((*graph)->edge->node));			
			
			if((*graph)->edge != NULL){
	
				(*graph)->edge = (*graph)->edge->next;

			}

			else break; 

	        }

		(*graph)->edge = temp;
		
	}


}

void set_pr_bool(graph_node** graph, bool value){ 


	graph_node* sag_aux = *graph;

	while(sag_aux != NULL){

		if(!strstr(sag_aux->data.node_name, "attackerLocated")) sag_aux->data.pr_set = value;
  		sag_aux = sag_aux->next;

       }

}

void set_current_pr(graph_node** graph){

	if((*graph) == NULL){
		return;
	}
	
	else{
		
		if((*graph)->data.vc_node || (*graph)->data.id == 1){

			(*graph)->data.pr_T = (*graph)->data.prev_pr_T;
			(*graph)->data.pr_F = (*graph)->data.prev_pr_F;

		}		

		graph_edge* edge_aux = (*graph)->edge;

		while(edge_aux != NULL){
			
			set_current_pr(&(edge_aux->node));			
			

			if((*graph)->edge != NULL){
	
				edge_aux = edge_aux->next;

			}

			else break; 

	        }

	}

}

void set_prev_pr(graph_node** graph){

	if((*graph) == NULL){
		return;
	}
	
	else{
		
		if((*graph)->data.vc_node || (*graph)->data.id == 1){

			(*graph)->data.prev_pr_T = (*graph)->data.pr_T;
			(*graph)->data.prev_pr_F = (*graph)->data.pr_F;

		}		

		graph_edge* edge_aux = (*graph)->edge;

		while(edge_aux != NULL){
			
			set_prev_pr(&(edge_aux->node));			
			

			if((*graph)->edge != NULL){
	
				edge_aux = edge_aux->next;

			}

			else break; 

	        }

	}
		
}

void print_predecessors(graph_node* node){

	printf("\n\nPredecessors: \n\n");

  	while(node != NULL){

		graph_edge* predecessor = node->predecessor_edge;

		printf("---------------------node id:%d\n\n", node->data.id); 
	
		while(predecessor != NULL){

			printf("%d ", predecessor->node->data.id);
			printf("%s ", predecessor->node->data.node_name);
			printf("pr_T: %lf ", predecessor->node->data.pr_T); 
			printf("pr_F: %lf\n\n", predecessor->node->data.pr_F); 

			predecessor = predecessor->next;

  		}

        	printf("\n\n----------------------------------------------------\n\n");

  		node = node->next;

  	}

}

void print_successors(graph_node* node){

  	printf("\n\nSuccessors: \n\n");

  	while(node != NULL){

		graph_edge* successor = node->edge;

		printf("---------------------node id:%d ", node->data.id); printf("edge count: %d\n\n", node->data.edge_count);
	
		while(successor != NULL){

			printf("%d ", successor->node->data.id);
			printf("%s \n\n",successor->node->data.node_name); 

			successor = successor->next;

  		}

        	printf("\n\n----------------------------------------------------\n\n");

  		node = node->next;

  	}

}

void print_graph_pr(graph_node* graph){

	graph_node* sag_aux = graph;

	while(sag_aux != NULL){

		if(!sag_aux->data.leaf_node || strstr(sag_aux->data.node_name, "attackerLocated")){

			printf("ID: %d ", sag_aux->data.id); printf(" Node name: %s ", sag_aux->data.node_name); 
	
			printf("pr T: %lf ", sag_aux->data.pr_T); 
			printf("pr F: %lf ", sag_aux->data.pr_F);

			if(sag_aux->data.vc_node) printf("VC NODE");
			if(sag_aux->data.vd_node) printf("VD NODE");

			printf("\n\n");

		}

  		sag_aux = sag_aux->next;

       }


}

void print_route(graph_node** graph){


	if((*graph) == NULL){
		return;
	}
	
	else{
		//printf("node id: %d\n", (*graph)->id);		

		graph_edge* temp = (*graph)->edge;

		while((*graph)->edge != NULL){

			if((*graph)->edge->path == false){

				(*graph)->edge->path = true;
				
				print_route(&((*graph)->edge->node));			
			}

			if((*graph)->edge != NULL){
	
				(*graph)->edge = (*graph)->edge->next;

			}

			else break; 

	        }

		(*graph)->edge = temp;
		
	}

}

void set_cve_score(graph_node** sag, graph_node** dag){ //Set cve scores

	graph_node* sag_aux = *sag;
	graph_node* dag_aux = *dag;


	while(dag_aux != NULL){ //set dag attacker node

		if(strstr(dag_aux->data.node_name, "attackerLocated")){

			dag_aux->data.pr_T = 0.7f;
			dag_aux->data.pr_F = 1 - dag_aux->data.pr_T;
					
		}	


		dag_aux = dag_aux->next;

	}


	dag_aux = *dag;	

	while(sag_aux != NULL){ //set sag pr values

		graph_edge* predecessor_aux = sag_aux->predecessor_edge;

		while(predecessor_aux != NULL){

			if(strstr(predecessor_aux->node->data.node_name, "vulExists")){

				printf("ID: %d ", sag_aux->data.id); 
  				printf("Node name: %s\n", predecessor_aux->node->data.node_name);
				printf("Enter Base Score: "); scanf("%lf",&predecessor_aux->node->data.pr_T);
		
				predecessor_aux->node->data.pr_T = predecessor_aux->node->data.pr_T/10;
				predecessor_aux->node->data.pr_F = 1 - predecessor_aux->node->data.pr_T;

				sag_aux->data.pr_T = predecessor_aux->node->data.pr_T;
				sag_aux->data.pr_F = 1 - sag_aux->data.pr_T;
				

			}

			predecessor_aux = predecessor_aux->next;

		}

		sag_aux = sag_aux->next;

	}

        sag_aux = *sag;

	while(dag_aux != NULL){ //set dag pr values

		while(sag_aux != NULL){

			if(dag_aux->data.id == sag_aux->data.id && !strstr(dag_aux->data.node_name, "attackerLocated")){

				dag_aux->data.pr_T = sag_aux->data.pr_T;
				dag_aux->data.pr_F = sag_aux->data.pr_F;
			}
			
			sag_aux = sag_aux->next;
			
		}

		sag_aux = *sag;
		dag_aux = dag_aux->next;	

	}	

}


void set_node_pr_T(graph_node** node){
	
	graph_edge* predecessor_aux = (*node)->predecessor_edge;

	while(predecessor_aux != NULL){ 

		if(strstr(predecessor_aux->node->data.node_name, "attackerLocated")){ 

			//P(B=T) = P(B=T)*P(A=T) -> "P(B=T)" is the initial bs score
			
			(*node)->data.pr_T *= predecessor_aux->node->data.pr_T; 
	
			predecessor_aux = predecessor_aux->next;

			continue;
			
		}

		if((*node)->data.vc_node && predecessor_aux->node->data.vd_node){ 
			
			(*node)->data.pr_T = (*node)->data.pr_T*predecessor_aux->node->data.pr_T; //P(X=T) = P(X=T)*P(X_2=T)*...*P(X_N=T) 
		}

		if((*node)->data.vd_node){ 

			//if((*node)->id == 1) printf("\npredecessor ID: %d ", predecessor_aux->node->id); 
			//printf("pr_T: %lf\n", predecessor_aux->node->pr_T);
		
			(*node)->data.pr_T *= 1 - predecessor_aux->node->data.pr_T; 
			//P(X=T) = 1 - (1-P(X=T))*(1-P(X_2=T))*...*(1-P(X_N=T))
			
		}

		predecessor_aux = predecessor_aux->next;

	}

	
	if((*node)->data.vd_node && !strstr((*node)->data.node_name, "attackerLocated")){

		(*node)->data.pr_T = 1 - (*node)->data.pr_T; //vd_node equation completes here for pr_T
		
	} 

}

void set_node_pr_F(graph_node** node){
	
	graph_edge* predecessor_aux = (*node)->predecessor_edge;

	if((*node)->data.vc_node && predecessor_aux->node->data.vd_node && 
		!strstr(predecessor_aux->node->data.node_name, "attackerLocated"))
		(*node)->data.pr_F = (*node)->data.pr_T;

	while(predecessor_aux != NULL){ 

		if(strstr(predecessor_aux->node->data.node_name, "attackerLocated")){  

			//P(B=F) = P(B=T)*P(A=T) -> "P(B=T)" is the pr score

			(*node)->data.pr_F = (*node)->data.pr_T * predecessor_aux->node->data.pr_T;

			predecessor_aux = predecessor_aux->next;

			continue;
			
		}

		if((*node)->data.vc_node && predecessor_aux->node->data.vd_node){ 
	
			(*node)->data.pr_F = (*node)->data.pr_F*predecessor_aux->node->data.pr_F; //P(X=F) = P(X=T)*P(X_2=F)*...*P(X_N=F)

		}

		if((*node)->data.vd_node){ 
					
			(*node)->data.pr_F *= 1 - predecessor_aux->node->data.pr_F; 
			//P(X=F) = 1 - (1-P(X=F))*(1-P(X_2=F))*...*(1-P(X_N=F))

		}

		predecessor_aux = predecessor_aux->next;

	}

	
	if((*node)->data.vd_node && !strstr((*node)->data.node_name, "attackerLocated")){
		
		(*node)->data.pr_F = 1 - (*node)->data.pr_F; //vd_node equation completes here for pr_F

	} 
	

}

void set_node_pr(graph_node** node){

	set_node_pr_T(node);
	set_node_pr_F(node);

}

void set_t(graph_node** graph){


	if((*graph) == NULL){
		
		return;
	}
	
	else{

		if((*graph)->data.vc_node) (*graph)->data.t_set = true;
		
		graph_edge* temp = (*graph)->edge;

		while((*graph)->edge != NULL){
	
			set_t(&((*graph)->edge->node));			
			
			if((*graph)->edge != NULL){
	
				(*graph)->edge = (*graph)->edge->next;

			}

			else break; 

	        }

		(*graph)->edge = temp;
		
	}

}

void calculate_descendant_t(graph_node** graph){

	if((*graph) == NULL){
		
		return;
	}
	
	else{

		set_node_pr(graph);
		
		graph_edge* temp = (*graph)->edge;

		while((*graph)->edge != NULL){

			if((*graph)->edge->path == false){

				(*graph)->edge->path = true;
				
				calculate_descendant_t(&((*graph)->edge->node));			
			}

			if((*graph)->edge != NULL){
	
				(*graph)->edge = (*graph)->edge->next;

			}

			else break; 

	        }

		(*graph)->edge = temp;
		
	}

}

void calculate_risk_prob(graph_node** graph){


	if((*graph) == NULL){
		
		return;
	}
	
	else{

		printf("\n\nID calculate 0: %d ", (*graph)->data.id);

		if(calculate_pr_bool((*graph)->predecessor_edge) && (*graph)->data.pr_set == false){ 

			printf("\n\nID calculate: %d ", (*graph)->data.id); printf("pr_T calculate: %lf \n", (*graph)->data.pr_T);

			set_node_pr(graph);
			(*graph)->data.pr_set = true;

			printf("\n\nID calculate 2: %d ", (*graph)->data.id); printf("pr_T calculate 2: %lf \n", (*graph)->data.pr_T);

		}		


		//printf("%d ", (*graph)->id);
		//printf("%s ",(*graph)->node_name); 
		
		//printf("pr_T: %lf ", (*graph)->pr_T);
		//printf("pr_F: %lf ", (*graph)->pr_F);
		//printf("\n\n");	

		graph_edge* temp = (*graph)->edge;

		//printf("edges: \n\n");

		while((*graph)->edge != NULL){

			//printf ("edge src: %d ", (*graph)->edge->src_id); printf ("edge dst: %d ", (*graph)->edge->dst_id);
			//printf("path: %d \n", (*graph)->edge->path);

			if((*graph)->edge->path == false){

				if(calculate_pr_bool((*graph)->predecessor_edge)) (*graph)->edge->path = true;
				
				calculate_risk_prob(&((*graph)->edge->node));			
			}

			if((*graph)->edge != NULL){
	
				(*graph)->edge = (*graph)->edge->next;

			}

			else break; 

	        }

		(*graph)->edge = temp;
		
	}

}

void set_abs_cum_risk(graph_node** node){ //cumulative risk probability and absolute risk probability

	graph_node* node_aux = *node;

	graph_edge* predecessor_aux = node_aux->predecessor_edge;

	while(node_aux != NULL){ //cumulative risk probability

		set_node_pr(&node_aux);

		/*predecessor_aux = node_aux->predecessor_edge;

		if(node_aux->vc_node){

			while(predecessor_aux != NULL && predecessor_aux->node->vd_node){

				node_aux->pr_T *= node_aux->pr_T*predecessor_aux->node->pr_T; //P(X=T) = P(X=T)*P(X_2=T)*...*P(X_N=T) 
				node_aux->pr_F *= node_aux->pr_T*predecessor_aux->node->pr_F; //P(X=F) = P(X=T)*P(X_2=F)*...*P(X_N=F) 			
				predecessor_aux = predecessor_aux->next;

			}

		}

		node_aux = node_aux->next;

	}

	node_aux = *node;

	while(node_aux != NULL){ //absolute risk probability

		predecessor_aux = node_aux->predecessor_edge;

		if(node_aux->vd_node){

			while(predecessor_aux != NULL){

				//P(X=T) = 1 - (1-P(X=T))*(1-P(X_2=T))*...*(1-P(X_N=T))	
				node_aux->pr_T *= 1 - predecessor_aux->node->pr_T; 
			
				//P(X=F) = 1 - (1-P(X=F))*(1-P(X_2=F))*...*(1-P(X_N=F))					
				node_aux->pr_F *= 1 - predecessor_aux->node->pr_F; 
				

				predecessor_aux = predecessor_aux->next;

			}

			node_aux->pr_T = 1 - node_aux->pr_T; //vd_node equation completes here for pr_T
			node_aux->pr_F = 1 - node_aux->pr_F; //vd_node equation completes here for pr_F		

		}*/


		node_aux = node_aux->next;

	}

}

cm_metric countermeasure_selection(graph_node** sag, alert a){

//------------------------------------------PART 1----------------------------------------------------//

	cm_metric optimal_cm = { .next = NULL}; cm_metric data = { .next = NULL};

	const int threshold = 8;

	graph_node* sag_aux = *sag;
	graph_node* root_node_pointer = sag_aux; //points allways to root node
	graph_node* target_node = sag_aux;

	while(sag_aux != NULL){ //find root node 

		if(strstr(sag_aux->data.node_name, "attackerLocated")){ //root node

			root_node_pointer = sag_aux;

		}

		sag_aux = sag_aux->next;

	}

	sag_aux = *sag;

	while(sag_aux != NULL){ //find target node (node with id equals to "1")

		if(sag_aux->data.id == 1){ //root node

			target_node = sag_aux;

		}

		sag_aux = sag_aux->next;

	}

	printf("\n\ntarget node 1: %lf", target_node->data.pr_T);
	
//------------------------------------------PART 2----------------------------------------------------//

	sag_aux = *sag;

	initialize_graph(sag);

	if(distance_to_target(&sag_aux, a) > threshold){ // node where alert was mapped to in acg

		printf("\nNo countermeasure will be selected");
		return optimal_cm;

	}

	else{ //set probability of V(alert) to "1" -> lines 7 and 8
		
		//should not be set by "set_prev_pr"		
		sag_aux->data.pr_T = 1.0f;
		sag_aux->data.pr_F = 0.0f;

		initialize_graph(sag);

		graph_node* t_set = sag_aux; //descendant(V-alert) U v-alert

		initialize_vd(&t_set);

		set_t(&t_set);

		calculate_descendant_t(&t_set); //calculate risk probability T

		printf("\n\ntarget node 2: %lf", target_node->data.pr_T);

		initialize_graph(sag);

		graph_edge* t_edge_aux = t_set->edge;

		while(t_edge_aux != NULL){
			
			set_prev_pr(&(t_edge_aux)->node);

			t_edge_aux = t_edge_aux->next;

		}

		printf("\n\ntarget node 3: %lf", target_node->data.pr_T);

		initialize_graph(sag);
							
	}

//------------------------------------------PART 3----------------------------------------------------//

	sag_aux = *sag;
	
	while(sag_aux != NULL){ // for each "t" belong to "T" -> line 10

		for(int i = 0; i<11; i++){ //for each cm in CM -> line 11

			if(sag_aux->data.t_set){ //to get all elements from t inside T

				//-----------------------------------IF CM.CONDITION------------------------------------//

				if(cm[i].condition){
	
					sag_aux->data.pr_T = sag_aux->data.pr_T * (1 - cm[i].effectiveness);

					graph_edge* edge_aux = sag_aux->edge;

					//-----------------------------------LOOP------------------------------------//

					while(edge_aux != NULL){ // all edges from "t" pointing to "descendant_t" nodes

						graph_node* descendant_t = edge_aux->node;

						initialize_graph(sag);
						initialize_vd(&descendant_t);

						calculate_descendant_t(&descendant_t); 

						data.node_id = sag_aux->data.id;
						data.cm_id = cm[i].id;
	
						//change in probability in target node

						//printf("\nprevious value: %lf ", target_node->prev_pr_T); 
						//printf("current value: %lf \n", target_node->pr_T);
 								
						data.value = target_node->data.pr_T - target_node->data.prev_pr_T; //benefit

						data.value = -1 * data.value;

						//printf("\nbenefit: %lf\n", data.value);
						
						insert_cm_metric(&benefit, data); //insert benefit
						
						set_current_pr(&descendant_t);

						edge_aux = edge_aux->next;
					}

					//-----------------------------------LOOP------------------------------------//

				}

				//-----------------------------------IF CM.CONDITION------------------------------------//
						
			}

		}

		sag_aux = sag_aux->next;

	}

//------------------------------------------PART 4----------------------------------------------------//

	sag_aux = *sag;

	while(benefit != NULL){ //for each cm in CM

		data.node_id = benefit->node_id;
		data.cm_id = benefit->cm_id;

		if((cm[benefit->cm_id].cost + cm[benefit->cm_id].intrusiveness) > 0){

			data.value = benefit->value/(cm[benefit->cm_id].cost + cm[benefit->cm_id].intrusiveness); //return of investment

			printf("\n\nROI value not zero: %lf\n\n", data.value);

			}

		else{
			data.value = 0;
			printf("\n\nROI value is 0.0\n\n");

		}

		//printf("\n\n");

  		//printf("Data Node id: %d ", data.node_id); printf("Data CM id: %d ", data.cm_id); 
		//printf("Data ROI: %lf\n\n", data.value); 

		insert_cm_metric(&ROI, data);

		benefit = benefit->next;

	}

//------------------------------------------PART 5----------------------------------------------------//
	
	sag_aux = *sag;
	double max_value = 0.0;
	int cm_id = 0;

	//cm_metric optimal_cm = { .next = NULL};

	while(ROI != NULL){

		printf("\nROI node %d ", ROI->node_id); 
		printf("ROI cm: %d ", ROI->cm_id); printf("ROI value: %lf\n\n", ROI->value);

		if(ROI->value > max_value){

			max_value = ROI->value;	

			optimal_cm.node_id = ROI->node_id;
			optimal_cm.cm_id = ROI->cm_id;
			optimal_cm.value = ROI->value;		

		}


		ROI = ROI->next;

	}

	return optimal_cm;

}

int main()
{

  /*IMPORTANT: Allways before calling another recursive graph method (print_route/calculate_risk_prob) you first need
  to initialize its path according to the instructions below:
  1- sag_aux = sag2; 
  2- initialize_graph_path(&sag_aux);
  3- print_route(&sag_aux); -> this one you should first make "sag_aux" point to attacker node
  4- sag_aux = sag2; -> initialize from first element
  5- initialize_graph_path(&sag_aux);*/

  srand ( time(NULL) );

  graph_node* sag = NULL;
  graph_node* dag = NULL;

  inicialization(&sag, "AttackGraph.txt"); //generate sag

  //graph_node* sag_aux = sag; 

  convert_to_dag(&dag, &sag);

  graph_node* dag_aux = dag;
 

//------------------Test DAG---------------------------------------//
	
	/*graph_node* dag = NULL;

	convert_to_dag(&dag, &sag_aux);  

	graph_node* dag_aux = dag;

	sag_aux = sag2;

	while(sag_aux != NULL){

		if(strstr(sag_aux->node_name, "hacl") || strstr(sag_aux->node_name, "networkServiceInfo")){ 

		//if(sag_aux->vc_node || sag_aux->leaf_node){
 
			printf("node id: %d ", sag_aux->edge->node->id); 
			printf("src node: %s ", sag_aux->edge->node->src_hostname); 
			printf("dst node: %s ", sag_aux->edge->node->dst_hostname);
			printf("protocol: %s ", sag_aux->edge->node->protocol); printf("port: %s\n\n",sag_aux->edge->node->port);

		}

		sag_aux = sag_aux->next;

	}

	sag_aux = sag2;

	while(dag_aux != NULL){

		printf("\n\n------------------Vulnerability Node--------------------------------\n\n");

		printf("\ndag id: %d ", dag_aux->id); printf("dag nome: %s ", dag_aux->node_name);
		printf("dag port: %s ", dag_aux->port); printf("dag protocol: %s\n", dag_aux->protocol);

		host_data* hosts = dag_aux->hosts;

		printf("\n\n------------------Hosts--------------------------------\n\n");

		while(hosts != NULL){

			printf("\nhost src: %s ", hosts->src_hostname);
			printf("host dst: %s\n", hosts->dst_hostname);

			hosts = hosts->next;

		}

		printf("\n\n--------------------------------------------------------\n\n");

		printf("\n\n----------------------------END------------------------------------\n\n");

		dag_aux = dag_aux->next;

	}*/

//-----------------Test if dag is complete------------//

	/*graph_node* dag = NULL;
	convert_to_dag(&dag, &sag_aux);  
	graph_node* dag_aux = dag;


	printf("\nprint node names part 1: \n");

	printf("\n");

	while(dag_aux != NULL){ // find attack node

		
		printf("ID: %d ", dag_aux->id); printf("node name: %s\n", dag_aux->node_name);
		dag_aux = dag_aux->next;

	}

	dag_aux = dag;

	printf("\n");

	printf("print node names part 2: \n\n");

	while(dag_aux != NULL){ // find attack node

		if(strstr(dag_aux->node_name, "attackerLocated")) 
			break;

		dag_aux = dag_aux->next;

	}

	print_graph(&dag_aux);*/


//----------------- Test if dag vc node is linked to dag vd node---------------//

	/*graph_node* dag = NULL;
	convert_to_dag(&dag, &sag_aux);  
	graph_node* dag_aux = dag;

	while(dag_aux != NULL){

		if(dag_aux->vc_node){


			printf("ID: %d ", dag_aux->id); printf("dag vc node: %s\n", dag_aux->node_name);
			printf("ID: %d ", dag_aux->edge->node->id); printf("dag vd node: %s\n", dag_aux->edge->node->node_name);

		}

		dag_aux = dag_aux->next;
	
	}*/

	
//--------------Print node's predecessors--------------------------//

  /*while(sag_aux != NULL){

	graph_edge* predecessors = sag_aux->predecessor_edge;

	printf("\n\n");

	printf("--------------------------------------------------------------------------------");

	printf("\nID: %d ", sag_aux->id); printf("Node name: %s\n\n", sag_aux->node_name);
	printf("Predecessors: \n");
	
	while(predecessors != NULL){
		
		printf("ID: %d ", predecessors->node->id); printf("%s\n", predecessors->node->node_name);
		predecessors = predecessors->next;

	}

	sag_aux = sag_aux->next;

  }

   sag_aux = sag2;*/
   
//-------------------Find initial node on graph-------------------//

  initialize_graph(&dag); 

  //printf("\n\nAfter removing cycles:: \n\n");

  //print_predecessors(dag);

  printf("\n\nAfter removing cycles:: \n\n");

  print_successors(dag);

  dag_aux = dag;
  
  set_cve_score(&sag, &dag); 

  while(dag_aux != NULL){

  	if(strstr(dag_aux->data.node_name, "attackerLocated"))		
		break;
	else
		dag_aux = dag_aux->next;

   }

   graph_node* dag_attacker_node = dag_aux;


//-------------------Set graph initial probabilities-------------------//

   int min_distance = 100000; int distance = 0;

   calculate_distance(&(dag_aux)->edge->node, &min_distance, distance);

   printf("\n\nDistance to target node: %d\n\n", min_distance);

   dag_aux = dag;

   initialize_graph(&dag);
   initialize_vd(&dag_attacker_node);

   printf("\n\nPART 0\n\n");

   print_dag(&dag_attacker_node); // dag_aux

   initialize_graph(&dag);
   initialize_vd(&dag_attacker_node);	

   printf("\n\nCALCULATE 0\n\n");

   calculate_risk_prob(&dag_attacker_node); // dag_aux

   //print_predecessors(dag);

   printf("\nPART 1\n\n");

   print_dag(&dag_attacker_node); //dag_aux

   initialize_graph(&dag);
   initialize_vd(&dag_attacker_node);

   printf("\n\nCALCULATE 1\n\n");

   calculate_risk_prob(&dag_attacker_node); //set cumulative risk probability and absolute risk probability / dag_aux

   printf("\nPART 2\n\n");

   print_dag(&dag_attacker_node); // dag_aux

   initialize_graph(&dag);

//-------------------Select countermeasures-------------------//

  alert a;
  
  //strcpy(a.src_hostname, "webServer");
  //strcpy(a.dst_hostname, "fileServer");

  //strcpy(a.src_hostname, "workStation");
  //strcpy(a.dst_hostname, "webServer");

  //strcpy(a.src_hostname, "webServer");
  //strcpy(a.dst_hostname, "webServer");

  //strcpy(a.src_hostname, "fileServer");
  //strcpy(a.dst_hostname, "webServer");

  //strcpy(a.src_hostname, "workStation");
  //strcpy(a.dst_hostname, "fileServer");

  //strcpy(a.src_hostname, "internet");
  //strcpy(a.dst_hostname, "webServer");

  //strcpy(a.src_hostname, "172.31.17.50");
  //strcpy(a.dst_hostname, "172.31.18.33");

  //strcpy(a.src_hostname, "172.31.17.74");
  //strcpy(a.dst_hostname, "172.31.27.71");

  strcpy(a.src_hostname, "internet");
  strcpy(a.dst_hostname, "172.31.17.74");

  cm_metric cm_data = countermeasure_selection(&dag, a);

  printf("\n\n");

  printf("Node id: %d ", cm_data.node_id); printf("CM id: %d ", cm_data.cm_id); printf("ROI: %lf\n\n", cm_data.value);

  return 0;

}    

