#ifndef SAG
#define SAG

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include "graph.h"

bool remove_edge = true;

void remove_char(char* word){ 

	if(strchr(word, '\'') == NULL) return;

   	int idxToDel = 0;

   	memmove(&word[idxToDel], &word[idxToDel + 1], strlen(word) - idxToDel);

   	word[strlen(word)-1] = 0;
	//word[strlen(word)*sizeof(char)] = 0;

}

void count(char *file_name, int counter){

	FILE* fptr = fopen(file_name, "r");

	int c; 
	while ( (c = fgetc (fptr)) != EOF )
		if ( c == '\n' )
    			counter++;

	printf("The file has %d lines", counter);

}

void set_string(char name[], char node_name[], const char* symbol_1, const char* symbol_2){

	char *sep; 

	strtol(node_name, &sep, 0);
	sep--;
	
	int position = strcspn(node_name, symbol_1)+1;
	int position2 = strcspn(node_name, symbol_2);

	strncpy(name, node_name + position, (node_name + position2) - (node_name + position));	

}

void insert_vc_node_parent(graph_node** parent_list, graph_node* parent){

	graph_node* temp = (graph_node*)malloc(sizeof(graph_node));
	
	temp->data.id = parent->data.id;
		
	strcpy(temp->data.node_name, parent->data.node_name);
	temp->data.node_name[sizeof(temp->data.node_name) - 1] = 0;
	
	strcpy(temp->data.src_hostname, parent->data.src_hostname);
	temp->data.src_hostname[sizeof(temp->data.src_hostname) - 1] = 0;

	strcpy(temp->data.dst_hostname, parent->data.dst_hostname);
	temp->data.dst_hostname[sizeof(temp->data.dst_hostname) - 1] = 0;
	
	temp->data.vd_node = parent->data.vd_node;

	temp->data.vc_node = parent->data.vc_node;
	temp->data.leaf_node = parent->data.leaf_node; 

	temp->edge = NULL;
		 								
	temp->next = *parent_list;
	*parent_list = temp;

}

void insert_host(host_data** node, host_data data){

	host_data* temp = (host_data*)malloc(sizeof(host_data));

	strcpy(temp->src_hostname, data.src_hostname);
	temp->src_hostname[sizeof(temp->src_hostname) - 1] = 0;

	strcpy(temp->dst_hostname, data.dst_hostname);
	temp->dst_hostname[sizeof(temp->dst_hostname) - 1] = 0;

	temp->next = *node;
	*node = temp;

}

void delete_hosts(host_data** head_ref){

	/* deref head_ref to get the real head */
	host_data* current = *head_ref;
   	host_data* next;

   	while (current != NULL) 
   	{

       		next = current->next;
       		free(current);
       		current = next;
       
   	}
   
   	/* deref head_ref to affect the real head back
      	in the caller. */
   	*head_ref = NULL;

}

bool search_predecessors(graph_edge* predecessors, int id){

	while(predecessors != NULL){
		
		if(predecessors->node->data.id == id) return true;	
		
		predecessors = predecessors->next;

	}

	return false;

}

bool search_node(graph_node** graph, int id){

	graph_node* node = *graph;

	while(*graph != NULL){
		
		if((*graph)->data.id == id) return true;	
		
		*graph = (*graph)->next;

	}

	*graph = node;	

	return false;

}

void insert_node(graph_node** node, graph_node payload){

	//remove_char(data.src_hostname); //removes \' from string ex: '192.168.25.34' -> 192.168.25.34
	//remove_char(data.dst_hostname);

	graph_node* temp = (graph_node*)malloc(sizeof(graph_node));
	
	temp->data.id = payload.data.id; 

	temp->data.prev_pr_T = 0;
	temp->data.prev_pr_F = 0;

	temp->data.pr_T = payload.data.pr_T;
	temp->data.pr_F = payload.data.pr_F;
		
	strcpy(temp->data.node_name, payload.data.node_name);
	temp->data.node_name[sizeof(temp->data.node_name) - 1] = 0;
	
	strcpy(temp->data.src_hostname, "EMPTY");
	temp->data.src_hostname[sizeof(temp->data.src_hostname) - 1] = 0;

	strcpy(temp->data.dst_hostname, "EMPTY");
	temp->data.dst_hostname[sizeof(temp->data.dst_hostname) - 1] = 0;

	strcpy(temp->data.port, "EMPTY");
	temp->data.port[sizeof(temp->data.port) - 1] = 0;

	strcpy(temp->data.protocol, "EMPTY");
	temp->data.protocol[sizeof(temp->data.protocol) - 1] = 0;
	
	temp->data.vd_node = payload.data.vd_node;

	temp->data.vc_node = payload.data.vc_node;
	temp->data.leaf_node = payload.data.leaf_node;
 
	temp->hosts = NULL;
	temp->edge = NULL;
	temp->parent_list = NULL;
	temp->predecessor_edge = NULL; 
		 								
	temp->next = *node;
	*node = temp;
			
	}

void delete_edge(graph_edge** head, int src_id, int dst_id){
    if (src_id == (*head)->src_id && dst_id == (*head)->dst_id) {
        graph_edge* temp = *head;
        *head = (*head)->next;
        free(temp);
        return;
    }

    graph_edge* current = (*head)->next;
    graph_edge* previous = *head;
    while (current != NULL && previous != NULL) {
        if (src_id == current->src_id && dst_id == current->dst_id) {
            graph_edge* temp = current;
            previous->next = current->next;
            free(temp);
            return;
        }
        previous = current;
        current = current->next;
    }
    return;
}

void insert_edge(graph_edge** edge, graph_node** node, int src, int dst){

	graph_edge* temp = (graph_edge*)malloc(sizeof(graph_edge));

	temp->src_id = src; 
	temp->dst_id = dst;

	temp->node = *node; //Node where edge points

	temp->path = false;

	temp->next = *edge;
	*edge = temp;
}

void dag_set_edges_number(graph_node** dag){

	graph_node* dag_aux = *dag;

	while(dag_aux != NULL){

		graph_edge* dag_edge = dag_aux->edge;

		int count = 0;

		while(dag_edge != NULL){

			count++;

			dag_edge = dag_edge->next;

		}

		dag_aux->data.edge_count = count;	
	
		dag_aux = dag_aux->next;

	}
}

void print_nodes(graph_node* graph){
	
	graph_node* temp = graph;
		
	while (temp != NULL){

		if(temp->data.vc_node == false){ //to print only vc nodes
			
			temp = temp->next;
			continue;

		}
						
		printf("%d ", temp->data.id);

		//puts(temp->node_name);

		printf("node name: %s \n", temp->data.node_name);

		//printf(" vd_node: %d", temp->vd_node);

		//printf(" edge number: %d", temp->edge_number);

		//printf(" alerts mapped: %d", temp->alerts_mapped);

		printf("src hostname: %s ", temp->data.src_hostname); printf("dst hostname: %s ", temp->data.dst_hostname);
				
		printf("\n");printf("\n");
		
		temp = temp->next;
			
	}	
	
}

void print_edges(graph_edge* edge){

	while(edge != NULL){

		printf("\nsrc id: %d ", edge->src_id); printf("dst id: %d ", edge->dst_id); printf("bool: %d ", edge->path);
		printf("pointing to: %d \n\n", edge->node->data.id);

		edge = edge->next;

	}

}

void set_predecessors_pr_set(graph_edge** p){

	graph_edge* predecessors = *p;

	while(predecessors != NULL){

		predecessors->node->data.pr_set = true;

		predecessors = predecessors->next;

	}


}

void initialize_vd(graph_node** graph){


	if((*graph) == NULL){
		
		return;
	}
	
	else{

		if((*graph)->data.vd_node && !strstr((*graph)->data.node_name, "attackerLocated")){ 
		
			(*graph)->data.pr_T = 1;
			(*graph)->data.pr_F = 1;

		}
		
		graph_edge* temp = (*graph)->edge;

		while((*graph)->edge != NULL){
	
			initialize_vd(&((*graph)->edge->node));			
			
			if((*graph)->edge != NULL){
	
				(*graph)->edge = (*graph)->edge->next;

			}

			else break; 

	        }

		(*graph)->edge = temp;
		
	}


}

void initialize_graph_path(graph_node** graph){

}

void initialize_graph(graph_node** graph){

	graph_node* sag_aux = *graph;

	while(sag_aux != NULL){ //initialize edges and nodes

		sag_aux->data.path = false;	//initialize nodes

		if(!strstr(sag_aux->data.node_name, "attackerLocated")) sag_aux->data.pr_set = false; //initialize node pr_set

		graph_edge* edge_aux = sag_aux->edge;

		while(edge_aux != NULL){

			edge_aux->path = false; //initialize edges

			edge_aux = edge_aux->next;

		}

		sag_aux = sag_aux->next;

	}
	
	sag_aux = *graph;

	while(sag_aux != NULL){ //initialize predecessor edges

		graph_edge* predecessor_edge = sag_aux->predecessor_edge;

		while(predecessor_edge != NULL){

			predecessor_edge->path = false;
	
			predecessor_edge = predecessor_edge->next;

		}

		sag_aux = sag_aux->next;

	}

}

void print_dag(graph_node** graph){

	if((*graph) == NULL){
		return;
	}
	
	else{
		printf("%d ", (*graph)->data.id);
		printf("%s ",(*graph)->data.node_name); 
		
		printf("pr_T: %lf ", (*graph)->data.pr_T);
		printf("pr_F: %lf", (*graph)->data.pr_F);
				
		printf("\n");

		graph_edge* temp = (*graph)->edge;

		while((*graph)->edge != NULL){
			
			print_dag(&((*graph)->edge->node));						

			if((*graph)->edge != NULL){
	
				(*graph)->edge = (*graph)->edge->next;

			}

			else break; 

	        }

		(*graph)->edge = temp;
		
	}

}

void remove_graph_cycles(graph_node** graph, graph_node** head){

	if((*graph) == NULL){
		return;
	}
	
	else{

		printf("\nID: %d\n\n", (*graph)->data.id);

		if((*graph)->edge != NULL && (*graph)->edge->path == true && (*graph)->data.edge_count > 1 /*&& remove_edge*/){

				printf("\nREMOVING CYCLES....\n\n");

				print_edges((*graph)->edge);

				//printf("node id: %d ", (*graph)->id); printf("node src edge: %d ", (*graph)->edge->src_id); 
				//printf("node dst edge: %d\n\n", (*graph)->edge->dst_id);

				graph_node* node = (*graph)->edge->node;

				delete_edge(&(node)->predecessor_edge, (*graph)->edge->dst_id, (*graph)->edge->src_id);

				delete_edge(&(*graph)->edge, (*graph)->edge->src_id, (*graph)->edge->dst_id);
				(*graph)->data.edge_count--;
	
				remove_edge = false;
	

		 }
		
		graph_edge* edge_aux = (*graph)->edge;

		while(edge_aux != NULL){

			if(edge_aux->path == false){

				edge_aux->path = true;
				
				remove_graph_cycles(&(edge_aux->node), head);	

				
			}

			if(edge_aux != NULL){

				initialize_graph(head);
	
				edge_aux = edge_aux->next;

			}

			else break; 

	        }

	}

}

void print_graph(graph_node** graph){

	if((*graph) == NULL){
		return;
	}
	
	else{
		
		printf("%d ", (*graph)->data.id);
		printf("%s ",(*graph)->data.node_name); 
		
		printf("pr_T: %lf ", (*graph)->data.pr_T);
		printf("pr_F: %lf", (*graph)->data.pr_F);
				
		printf("\n");

		graph_edge* temp = (*graph)->edge;

		while((*graph)->edge != NULL){

			if((*graph)->edge->path == false){

				(*graph)->edge->path = true;
				
				print_graph(&((*graph)->edge->node));			
			}

			if((*graph)->edge != NULL){
	
				(*graph)->edge = (*graph)->edge->next;

			}

			else break; 

	        }

		(*graph)->edge = temp;
		
	}

}


void add_edge(graph_node **graph, int src, int dst){

	graph_node* temp = *graph;
	graph_node* temp2 = *graph;

	while(*graph != NULL){

		if((*graph)->data.id == src){ //search for edge source
			
			while(temp2 != NULL){ 

				if(temp2->data.id == dst){ //search for edge destination

					insert_edge(&((*graph)->edge), &temp2, src, dst);
					insert_edge(&((*graph)->edge->node->predecessor_edge), graph, dst,src);
								
					break;

				}

				temp2 = temp2->next;

			}

			temp2 = temp;
			break;

		}

		*graph = (*graph)->next;

	}	
	
	*graph = temp;

}

int generate_nodes(graph_node **graph, const char *file_name)
{
    if(file_name == NULL || graph == NULL)
        return 0; // return error
	

    // no need to allocate memory for it
    // if the insert_node is going to make a
    // copy anyway
    struct Graph_Node payload = { .next = NULL, .edge = NULL, .hosts = NULL, .parent_list = NULL, .predecessor_edge = NULL};

    FILE *fp = fopen(file_name, "r");
    if(fp == NULL)
    {
        fprintf(stderr, "Error opening file %s: %s\n", file_name,
                strerror(errno));
        return 0;
    }

    // no line will be longer than 1024
    // based on your conditions
    char line[1024];

    size_t linenmr = 0;

    while(fgets(line, sizeof line, fp))
    {
        linenmr++;
        // getting rid of the newline
        line[strcspn(line, "\n")] = 0;

	if(strstr(line, "AND") == NULL && strstr(line, "OR") == NULL && strstr(line, "LEAF") == NULL)
		continue;

	if(strstr(line, "AND") != NULL)
		payload.data.vc_node = true;
	else
		payload.data.vc_node = false;

	if(strstr(line, "OR") != NULL)
		payload.data.vd_node = true;
	else
		payload.data.vd_node = false;

	if(strstr(line, "LEAF") != NULL)
		payload.data.leaf_node = true;
	else
		payload.data.leaf_node = false;

        char *sep;
        long int id = strtol(line, &sep, 0);

        // assuming that there is not white spaces
        // before and after the commas
        if(*sep != ',')
        {
            fprintf(stderr, "Warning, line %lu is malformatted, '<id>,' exepcted\n", linenmr);
            continue;
        }

        payload.data.id = id;

        // format is: "....",

        if(sep[1] != '"')
        {
            fprintf(stderr, "Warning, line %lu is malformatted, \"<string>\", exepcted\n", linenmr);
            continue;
        }

        // looking for ",
        char *endname = strstr(sep + 2, "\","); 

        if(endname == NULL)
        {
            fprintf(stderr, "Warning, line %lu is malformatted, \"<string>\", exepcted\n", linenmr);
            continue;
        }

        // ending string at ",
        // easier to do strcnpy
        *endname = 0;

        strncpy(payload.data.node_name, sep + 2, sizeof payload.data.node_name);
        payload.data.node_name[sizeof(payload.data.node_name) - 1] = 0;

	payload.data.pr_T = 1.0f; payload.data.pr_F = 1.0f;
	
        insert_node(graph, payload);

	memset(payload.data.node_name, 0, sizeof(payload.data.node_name));	
    }

    fclose(fp);
    return 1;
}

int generate_edges(graph_node **graph, const char *file_name){

	FILE *fp = fopen(file_name, "r");

	if(fp == NULL)
    	{
        fprintf(stderr, "Error opening file %s: %s\n", file_name,
                strerror(errno));
        return 0;
    	}

    	char line[1024];

    	size_t linenmr = 0;

    	while(fgets(line, sizeof line, fp))
    	{

		linenmr++;
        	// getting rid of the newline
        	line[strcspn(line, "\n")] = 0;

		if(strstr(line, "AND") != NULL || strstr(line, "OR") != NULL || strstr(line, "LEAF") != NULL)
			continue;

		/*if(strlen(line) > 12)
	            	continue; // resume reading, skip the line*/

		char *sep;
        	long int dst = strtol(line, &sep, 0);

		sep++;

		long int src = strtol(sep, &sep, 0);

		//search for edge in graph

		//printf("dst: %ld ", dst); printf("src: %ld\n", src);

		add_edge(graph, src, dst);

    	}

	return 1;

}

void set_ip(char node_name[], char ip[]){

	if(strstr(node_name, "hacl") != NULL){

		char* sep;
		sep = node_name;

		int comma_num = 0;
		int i = 0;

		while(comma_num < 2){

			if(*sep == ',')
				comma_num++;

			sep++;

			if(comma_num > 0 && comma_num < 2){
			
				if(*sep != ','){
					
					ip[i] = *sep;
					i++;				
				}			

			}
				

		}

		ip[i] = 0;

		remove_char(ip); 
	} 

	else if(strstr(node_name, "networkServiceInfo") != NULL){

		set_string(ip, node_name, "(", ",");

		remove_char(ip);

	}

}

void set_vc_hosts(graph_node** vc_node, graph_node* leaf_node){

	if(strstr(leaf_node->data.node_name, "hacl") != NULL){ // hacl

		memset((*vc_node)->data.src_hostname, 0, sizeof((*vc_node)->data.src_hostname));
		
		memset((*vc_node)->data.dst_hostname, 0, sizeof((*vc_node)->data.dst_hostname));

		set_string((*vc_node)->data.src_hostname, leaf_node->data.node_name, "(", ",");

		char* sep;
		sep = leaf_node->data.node_name;

		int comma_num = 0;
		int i = 0;

		while(comma_num < 2){

			if(*sep == ',')
				comma_num++;

			sep++;

			if(comma_num > 0 && comma_num < 2){
			
				if(*sep != ','){
					
					(*vc_node)->data.dst_hostname[i] = *sep;
					i++;				
				}			

			}
				

		}

		(*vc_node)->data.dst_hostname[i] = 0;

		remove_char((*vc_node)->data.src_hostname); 
		remove_char((*vc_node)->data.dst_hostname); 	
		
	} 
	
	else{

		if(strstr(leaf_node->data.node_name, "attackerLocated") != NULL){

			memset((*vc_node)->data.src_hostname, 0, sizeof((*vc_node)->data.src_hostname));

			set_string((*vc_node)->data.src_hostname, leaf_node->data.node_name, "(", ")");
			remove_char((*vc_node)->data.src_hostname); 

		}else{

			memset((*vc_node)->data.src_hostname, 0, sizeof((*vc_node)->data.src_hostname));

			set_string((*vc_node)->data.src_hostname, leaf_node->data.node_name, "(", ",");
			remove_char((*vc_node)->data.src_hostname);

		}
	}

}

void inicialization(graph_node** sag, char* file_name){ 

	generate_nodes(sag, file_name);
	generate_edges(sag, file_name);

	graph_node* temp = *sag; //leaf nodes or vd_nodes
	graph_node* temp2 = *sag; //vc nodes

	//Set node vc hosts

	while(temp != NULL){ //set leaf nodes----------

		if(temp->data.leaf_node){

			while(temp2 != NULL){

				graph_edge* edge_aux = temp->edge; //--HERE

				while(temp->edge != NULL){

					if(temp2->data.vc_node && temp2->data.id == temp->edge->dst_id){
										
						set_vc_hosts(&temp2, temp); //Set vc hostnames

					}
				
					temp->edge = temp->edge->next;

				}

				temp->edge = edge_aux; 

				//--HERE

				temp2 = temp2->next;				

			}

			temp2 = *sag;	
		
		}

		temp = temp->next;

	} //--------------------------------------------------------

	temp = *sag;

	while(temp != NULL){ //set vd nodes----------

		if(temp->data.vd_node){

			while(temp2 != NULL){

				graph_edge* edge_aux = temp->edge; //--HERE

				while(temp->edge != NULL){

					if(temp2->data.vc_node && temp2->data.id == temp->edge->dst_id){

						if(strstr(temp2->data.src_hostname, "EMPTY") != NULL) //There is "EMPTY" string
							set_vc_hosts(&temp2, temp); //Set vc hostnames

					}

					temp->edge = temp->edge->next;

				}
				
				temp->edge = edge_aux;

				//--HERE

				temp2 = temp2->next;				

			}

			temp2 = *sag;	
		
		}

		temp = temp->next;

	} //--------------------------------------------------------

	temp = *sag;

	while(temp != NULL){ 

		if(temp->data.vc_node && strstr(temp->data.dst_hostname, "EMPTY") != NULL){

			memset(temp->data.dst_hostname, 0, sizeof(temp->data.dst_hostname));
			strcpy(temp->data.dst_hostname, temp->data.src_hostname);

		}

		temp = temp->next;

	}

	//Set vc nodes parents-----------------------------------

	temp = *sag;

	while(temp != NULL){

		if(temp->data.vc_node && temp->edge != NULL){ //temp -> vc_node
		
			graph_node* vd_node = temp->edge->node; //vd node

			if(vd_node->edge != NULL){	
		
				graph_edge* edge_aux = vd_node->edge;

				while(vd_node->edge != NULL){

					graph_node* vc_node_child = vd_node->edge->node; //child vc_node
					insert_vc_node_parent(&(vc_node_child)->parent_list, temp);

					vd_node->edge = vd_node->edge->next;
					 	
				}
			
				vd_node->edge = edge_aux;

			}

		}

		temp = temp->next;

	}
 
}

void link_dag_attacker_node(graph_node** dag_node, graph_node** attacker_dag_node){

	graph_node* head = *dag_node;
	graph_node* dag_aux = *dag_node;

	while(*dag_node != NULL){

		bool link = true;

		while(dag_aux != NULL){

			if(dag_aux->data.vd_node){

				graph_edge* dag_vd_edge = dag_aux->edge;

				while(dag_vd_edge != NULL){

					if((*dag_node)->data.id == dag_vd_edge->node->data.id) link = false;
   						                					
					dag_vd_edge = dag_vd_edge->next;

				}

			}

			
			dag_aux = dag_aux->next;

		}
	
		if(link == true && (*dag_node)->data.vc_node){ //vc node has no predecessor 

			insert_edge(&(*attacker_dag_node)->edge, dag_node, head->data.id, (*dag_node)->data.id);
			insert_edge(&(*dag_node)->predecessor_edge, attacker_dag_node, (*dag_node)->data.id, head->data.id);
		}

		link = true;
		dag_aux = head;		
				
		*dag_node = (*dag_node)->next;

	}


}

void link_dag_nodes(graph_node** dag, graph_node** graph){

	graph_node* dag_node = *dag;
	graph_node* dag_vd_node = NULL; 

	while(dag_node != NULL){ // search dag vc node

		if(dag_node->data.vd_node){

			dag_vd_node = dag_node;

			graph_edge* dag_vd_edge = dag_node->edge;

			 while(dag_vd_edge != NULL){

				if(dag_vd_node->data.id == (*graph)->data.id){ //find dag vd node on graph

					graph_edge* graph_vd_edge = (*graph)->edge;

					while(graph_vd_edge != NULL){ //find nodes the "graph_vd_edge" is pointing to

						graph_node* dag_vc_node = *dag;

						graph_node* graph_vc_node = graph_vd_edge->node; //node "graph_vd_edge" is pointing to
			
						while(dag_vc_node != NULL){ //find node where "graph_vd_edge" is pointing to on dag 

							if(dag_vc_node->data.id == graph_vc_node->data.id && !dag_vc_node->data.path){

								dag_vc_node->data.path = true;	 
							
								//insert edge -> from dag vd node to dag vc node
								insert_edge(&(dag_vd_node)->edge, &dag_vc_node, dag_vd_node->data.id, 									dag_vc_node->data.id);

								//insert predecessor -> from dag vc node to dag vd node
								insert_edge(&(dag_vc_node)->predecessor_edge, &dag_vd_node, 
								dag_vc_node->data.id, dag_vd_node->data.id); 
																																									
							}

							dag_vc_node = dag_vc_node->next;

						  }
						
						graph_vd_edge = graph_vd_edge->next;

					  }


				   }			
		
				
				dag_vd_edge = dag_vd_edge->next;

			 }
			
		}
			
		dag_node = dag_node->next;
	}

}

void set_dag_vc_node(graph_node** dag, graph_node** dag_vc_node, graph_node** graph){

	if((*graph)->edge == NULL){

		return;
	}
	
	else{
	
		if((*graph)->data.vc_node){ // First sag vc node found after function call -> *graph == sag_vd_node -> on first call

			graph_node* dag_aux = *dag;

			while(dag_aux != NULL){
			
				//if sag_vc_node "(*graph)->id" is present in "dag_aux"  
				if(dag_aux->data.id == (*graph)->data.id && *dag_vc_node == NULL) *dag_vc_node = dag_aux;
	
				dag_aux = dag_aux->next;

			}

		}

		graph_edge* temp = (*graph)->edge;

		while((*graph)->edge != NULL){

			if((*graph)->edge->path == false){

				(*graph)->edge->path = true;
				
				set_dag_vc_node(dag, dag_vc_node, &((*graph)->edge->node));			
			}

			if((*graph)->edge != NULL){
	
				(*graph)->edge = (*graph)->edge->next;

			}

			else break; 

	        }

		(*graph)->edge = temp;
		
	}

}



void build_dag(graph_node** dag, graph_node** graph){

	graph_node* sag_aux = *graph;
	
	while(sag_aux != NULL){

		if(sag_aux->data.vd_node){

			graph_node* dag_aux = *dag;

			while(dag_aux != NULL){

				if(dag_aux->data.id == sag_aux->data.id && dag_aux->data.id != 1){
					
					graph_node* dag_vd_node = dag_aux;
					graph_edge* sag_vd_edge = sag_aux->edge;
				
					while(sag_vd_edge != NULL){

						graph_node* dag_vc_node = NULL;

						initialize_graph(graph);

						set_dag_vc_node(dag, &dag_vc_node, &(sag_vd_edge)->node);

						if(!search_predecessors((dag_vd_node)->predecessor_edge, dag_vc_node->data.id)){
						
							//insert edge -> from dag vd node to dag vc node
							insert_edge(&(dag_vd_node)->edge, &dag_vc_node, dag_vd_node->data.id, 
							dag_vc_node->data.id);

							//insert predecessor -> from dag vc node to dag vd node
							insert_edge(&(dag_vc_node)->predecessor_edge, &dag_vd_node, dag_vc_node->data.id, 
							dag_vd_node->data.id);

						}

						sag_vd_edge = sag_vd_edge->next;

					}

				}

				dag_aux = dag_aux->next;

			}

		}

		sag_aux = sag_aux->next;

	}

}

void set_port_protocol(char node_name[], char protocol[], char port[]){

	char* sep;
	sep = node_name;

	int comma_num = 0;
	int i = 0; int k =0;

	//printf("node name: %s\n\n", node_name);

	while(comma_num < 4){

		if(*sep == ')') break;

		if(*sep == ',')
			comma_num++;

		sep++;

		if(comma_num == 2){
			
			if(*sep != ',' && *sep != ')'){
					
				protocol[i] = *sep;
				i++;				
			}			

		}

		if(comma_num == 3){
			
			if(*sep != ',' && *sep != ')'){
					
				port[k] = *sep;
				k++;				
			}			

		}
				
	}

	protocol[i] = 0; port[k] = 0;

	remove_char(protocol); 
	remove_char(port); 	

}

void convert_to_dag(graph_node** dag, graph_node** sag){ //directed acyclic graph

	struct Graph_Node payload = { .next = NULL, .edge = NULL, .hosts = NULL, .predecessor_edge = NULL, .parent_list = NULL};

	graph_node* sag_aux = *sag;

//---------------------------SET PORTS AND SERVICES------------------------------//

	while(sag_aux != NULL){ //set vc nodes ports and services

		// service leaf node
		if(strstr(sag_aux->data.node_name, "networkServiceInfo") || strstr(sag_aux->data.node_name, "hacl")){  
		
			//printf("\n\nnode name: %s\n\n", sag_aux->node_name);
			
			graph_edge* edge_aux = sag_aux->edge;

			//printf("Part 1\n\n");

			while(edge_aux != NULL){ 

				char protocol[50]; char port[50]; char ip[50];

				memset(protocol, 0, sizeof(protocol)); memset(port, 0, sizeof(port)); memset(ip, 0, sizeof(ip));

				set_port_protocol(sag_aux->data.node_name, protocol, port); //extract port and protocol from leaf node

				set_ip(sag_aux->data.node_name, ip); //extract ip from leaf node

				graph_node* vc_node = edge_aux->node; // leaf pointing to vc node

				//set leaf protocol, port and ip
				strcpy(sag_aux->data.protocol, protocol); strcpy(sag_aux->data.port, port); strcpy(sag_aux->data.ip, ip);
 
				//copy to vc node
				strcpy(vc_node->data.protocol, protocol); strcpy(vc_node->data.port, port); strcpy(vc_node->data.ip, ip);

				/*if(strstr(sag_aux->node_name, "hacl")){
					
					printf("\nSAG ID: %d ", sag_aux->id); 
					printf("SAG IP: %s\n\n", sag_aux->ip);
				}*/ 

				edge_aux = edge_aux->next;

			}

			//printf("node name 2: %s ", sag_aux->edge->node->node_name);
			//printf("protocol: %s ", sag_aux->edge->node->protocol); 
			//printf("port: %s\n\n",sag_aux->edge->node->port);

			//printf("Part 2\n\n"); 

		}

		sag_aux = sag_aux->next;

	}

//---------------------------INSERT DAG VC NODES------------------------------//

	sag_aux = *sag;  
	
	graph_node* leaf_node = *sag;
	graph_node* hacl_node = *sag;

	while(leaf_node != NULL){

		hacl_node =*sag;

		//leaf node linked to vulnerability node	
		if(strstr(leaf_node->data.node_name, "networkServiceInfo") && leaf_node->edge != NULL){

			graph_edge* leaf_edge = leaf_node->edge;

			while(leaf_edge != NULL){

				//insert dag node

				graph_node payload = { .next = NULL, .edge = NULL, .hosts = NULL, .parent_list = NULL, 
					.predecessor_edge = NULL };

				graph_node* vc_node = leaf_edge->node;

				payload.data.id = vc_node->data.id; 
				payload.data.pr_T = vc_node->data.pr_T; payload.data.prev_pr_T = vc_node->data.prev_pr_T; 
		        	payload.data.pr_F = vc_node->data.pr_F; payload.data.prev_pr_F = vc_node->data.prev_pr_F;
			
				payload.data.vc_node = true;

				strcpy(payload.data.node_name, vc_node->data.node_name);

				insert_node(dag, payload); //insert vulnerability node

				memset((*dag)->data.port, 0, sizeof((*dag)->data.port)); 
				memset((*dag)->data.protocol, 0, sizeof((*dag)->data.protocol));
				memset((*dag)->data.ip, 0, sizeof((*dag)->data.ip));

				strcpy((*dag)->data.port, leaf_node->data.port); strcpy((*dag)->data.protocol, leaf_node->data.protocol);
				strcpy((*dag)->data.ip, leaf_node->data.ip);

				leaf_edge = leaf_edge->next;

			} 

			while(hacl_node != NULL){

				if(strstr(hacl_node->data.node_name, "hacl")){ //set hosts to vulnerability node

					/*if(hacl_node->id == 25){ 

						printf("NODE 25\n\n");
						printf("hacl id: %d ", hacl_node->id); 
						printf("name: %s ", hacl_node->node_name);
						printf("port: %s ", hacl_node->port);
						printf("protocol: %s\n\n", hacl_node->protocol);					
				
					}*/

					if(strcmp(hacl_node->data.port, (*dag)->data.port) == 0 
						&& strcmp(hacl_node->data.protocol, (*dag)->data.protocol) == 0
						&& strcmp(hacl_node->data.ip, (*dag)->data.ip) == 0){

						host_data host;

						memset(host.src_hostname, 0, sizeof(host.src_hostname));
						memset(host.dst_hostname, 0, sizeof(host.dst_hostname));

						strcpy(host.src_hostname, hacl_node->edge->node->data.src_hostname); 
						strcpy(host.dst_hostname, hacl_node->edge->node->data.dst_hostname);

						insert_host(&(*dag)->hosts, host);							

					}
									
				}

				hacl_node = hacl_node->next;

			}			

		}

		leaf_node = leaf_node->next;

	}

//-------------------------------INSERT DAG VD NODE---------------------------------// --> Test this part


	graph_node* sag_vc_node = *sag;
	graph_node* dag_node = *dag;

	while(dag_node != NULL){

		graph_node payload = { .next = NULL, .edge = NULL, .hosts = NULL, .parent_list = NULL, .predecessor_edge = NULL };

		sag_vc_node = *sag;

		while(sag_vc_node != NULL){

			if(sag_vc_node->data.id == dag_node->data.id && sag_vc_node->data.vc_node){

				//insert vd_node

				graph_node* dag_vc_node = dag_node;
				graph_node* dag_vd_node = *dag;

				graph_edge* sag_vc_edge = sag_vc_node->edge;

				while(sag_vc_edge != NULL){ 

					graph_node* sag_vd_node = sag_vc_edge->node;
		
					payload.data.id = sag_vd_node->data.id;
					
					payload.data.pr_T = sag_vd_node->data.pr_T; 
					payload.data.prev_pr_T = sag_vd_node->data.prev_pr_T;
 
					payload.data.pr_F = sag_vd_node->data.pr_F; 
					payload.data.prev_pr_F = sag_vd_node->data.prev_pr_F;	
				
					memset(payload.data.node_name, 0, sizeof(payload.data.node_name));	
					strcpy(payload.data.node_name, sag_vd_node->data.node_name);

					payload.data.vd_node = true;

					if(search_node(&dag_vd_node, sag_vd_node->data.id) == false){

						insert_node(dag, payload); //insert vd node
						dag_vd_node = *dag;
					}else{

						graph_node* dag_aux = *dag;

						while(dag_aux != NULL){

							if(dag_aux->data.id == sag_vd_node->data.id) dag_vd_node = dag_aux;	

							dag_aux = dag_aux->next;

						}

					
					} 

					//insert edge
					insert_edge(&(dag_vc_node)->edge, &dag_vd_node, dag_vc_node->data.id, dag_vd_node->data.id); 

					//insert predecessor
					insert_edge(&(dag_vd_node)->predecessor_edge, &dag_vc_node, dag_vd_node->data.id, 
					dag_vc_node->data.id);

					dag_vd_node = *dag; // dag_vd_node should point to the first node for the next loop

					sag_vc_edge = sag_vc_edge->next;

				}	

				//--insert vd_node

			}

			
			sag_vc_node = sag_vc_node->next;

		}

		dag_node = dag_node->next; 		
		
	}

//--------------------------------LINK DAG NODES-----------------------------------//


	sag_aux = *sag; dag_node = *dag;
	build_dag(&dag_node, &sag_aux);

	sag_aux = *sag; 

        graph_node* dag_vd_node = *dag; dag_node = *dag;

	graph_node* attacker_sag_node = *sag;
	graph_node* root_sag_node = *sag;

	while(attacker_sag_node != NULL){ // find attack node

		if(strstr(attacker_sag_node->data.node_name, "attackerLocated")) 
			break;

		attacker_sag_node = attacker_sag_node->next;

	}

	while(root_sag_node != NULL){ // find root node -> last not like in Nice paper

		if(root_sag_node->data.id == 1) 
			break;

		root_sag_node = root_sag_node->next;

	}

	payload.data.vd_node = false; payload.data.vc_node = false; payload.data.leaf_node = false; 
	payload.data.start_node = false; payload.data.path = false; 
	payload.data.pr_set = false;

	payload.data.id = attacker_sag_node->data.id;

	memset(payload.data.node_name, 0, sizeof(payload.data.node_name));	
	strcpy(payload.data.node_name, attacker_sag_node->data.node_name);
	payload.data.vd_node = true;

	insert_node(dag, payload); //add attacker node on dag

	graph_node* attacker_dag_node = *dag;

	//---Link attacker node to initial dag vc nodes

	link_dag_attacker_node(&dag_node, &attacker_dag_node);

	//---Link last vc nodes to root vd node

	memset(payload.data.node_name, 0, sizeof(payload.data.node_name));

	payload.data.id = root_sag_node->data.id;		
	strcpy(payload.data.node_name, root_sag_node->data.node_name);

	payload.data.vd_node = true; payload.data.vc_node = false; payload.data.leaf_node = false; 
	payload.data.start_node = false; payload.data.path = false; 
	payload.data.pr_set = false;
	
	graph_node* root_dag_node = *dag;

	dag_node = *dag; dag_vd_node = *dag;

	if(search_node(&root_dag_node, 1) == false){ // value "1" to find root node

		insert_node(dag, payload); //add root node on dag
		root_dag_node = *dag;
	}

	while(dag_node != NULL){

		if(dag_node->edge == NULL && dag_node->data.vc_node && dag_node->data.id != 1){
			
			insert_edge(&(dag_node)->edge, &root_dag_node, dag_node->data.id, root_dag_node->data.id);
			insert_edge(&(root_dag_node)->predecessor_edge, &dag_node, root_dag_node->data.id, dag_node->data.id);	
		}

		dag_node = dag_node->next;

	}

	/*dag_node = *dag;
	
	printf("\n\nPredecessors before removing cycles: \n\n");

  	while(dag_node != NULL){

		graph_edge* predecessor = dag_node->predecessor_edge;

		printf("---------------------node id:%d \n\n", dag_node->id); 
	
		while(predecessor != NULL){

			printf("%d ", predecessor->node->id);
			printf("%s \n\n",predecessor->node->node_name); 

			predecessor = predecessor->next;

  		}

        	printf("\n\n----------------------------------------------------\n\n");

  		dag_node = dag_node->next;

  	}*/

        dag_node = *dag;

	// Remove cycles from dag

	dag_set_edges_number(dag);

	//Before removing cycles

	dag_node = attacker_dag_node;

	dag_node->data.pr_set = true;

	//remove_graph_cycles(&dag_node, dag);

	//After removing cycles

	dag_node = *dag;

	link_dag_attacker_node(&dag_node, &attacker_dag_node);	

	dag_set_edges_number(dag);
	
}	

#endif	

