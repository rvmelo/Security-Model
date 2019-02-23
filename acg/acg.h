#ifndef ACG
#define ACG


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>

#include "graph.h"
#include "Data.h"
#include "sag.h"

#define THRESHOLD 2

void delete_acg_path(alert_path** acg, alert_path* path){

	if (*acg == path) {
        
		alert_path* head_ref = *acg;
        	*acg = (*acg)->next_path;

		alert_path* current = head_ref;
   		alert_path* next;

		while(current != NULL){
        	
			next = current->next;
       			free(current);
       			current = next;

		}
        
		return;
        
	}

	alert_path* current = (*acg)->next_path;
   	alert_path* previous = *acg;

	while (current != NULL && previous != NULL){

		if(path == current){

			alert_path* head_ref = current;
			previous->next_path = current->next_path;

			alert_path* current_aux = head_ref;
   			alert_path* next;

			while(current_aux != NULL){
        	
				next = current_aux->next;
       				free(current_aux);
       				current_aux = next;

			}
            		
			return;

		}

		previous = current;
        	current = current->next;

	}


}


int alert_outside_network(alert a){

	if(strcmp(a.src_hostname, "172.31.29.192") == 0 || strcmp(a.src_hostname, "172.31.18.33") == 0 || 
	strcmp(a.src_hostname, "172.31.27.3") == 0 || strcmp(a.src_hostname, "172.31.46.216") == 0 || 
	strcmp(a.src_hostname, "172.31.30.80") == 0 || strcmp(a.src_hostname, "172.31.22.208") == 0 || 
	strcmp(a.src_hostname, "172.31.17.74") == 0 || strcmp(a.src_hostname, "172.31.22.238") == 0)

		return 0;
	else
		return 1;

}	
	
void insert_alert_acg(alert_path **Si, alert a){

	alert_path* temp = (alert_path*)malloc(sizeof(alert_path));

	strcpy(temp->src_hostname, a.src_hostname);
	strcpy(temp->dst_hostname, a.dst_hostname);

	temp->seconds_elapsed = a.seconds_elapsed;

	temp->next_path = NULL;

	if(*Si == NULL){

		temp->next = NULL;
		*Si = temp;

	}else{

		alert_path* temp2 = *Si;

		while(1){ 

			if((*Si)->next == NULL){

				temp->next = NULL;
				(*Si)->next = temp;
				*Si = temp2;	
				break;

			}
				
			*Si = (*Si)->next;

		}

	}

}


void print_acg(alert_path* acg){

	int i = 0;

	while(acg != NULL){

		i++;
		
		alert_path* path_aux = acg;

		printf("Path %d:\n\n", i);

		while(path_aux != NULL){

			printf("src alert: %s ", path_aux->src_hostname);
			printf("dst alert: %s\n", path_aux->dst_hostname);
			
			path_aux = path_aux->next;

		}

		printf("\n");
                
		acg = acg->next_path;

	}

}

alert_path *duplicate_path(alert_path *path, alert_path *path_aux)
{
     alert_path*  root_elem = 0;
     alert_path* preElem = 0;

     // Move through all the nodes in source list.
     while(path != 0) {

          // Create the memory for the destination list node.
          alert_path *elem = (alert_path*)malloc (sizeof(alert_path));

          if(elem == 0) {
               printf("Failed allocation!"), exit(1);
          }

          // Copy the data from the source to dest node
	  strcpy(elem->src_hostname, path->src_hostname);
	  strcpy(elem->dst_hostname, path->dst_hostname);		
	 
	  elem->seconds_elapsed = path->seconds_elapsed;
	  elem->next = 0;

          if (root_elem == 0) {
                // keep the root for the destination list.
                root_elem = elem;
                preElem = elem;
          }
          else {
                // point the newly created node to the previous created node.
                preElem->next = elem;
                // keep the new node to use it as previous node in next node creation.
                preElem = elem;
          }

	  if(path == path_aux){ 
		elem->next = NULL; 
		break;
	  }

          path = path->next;
    }

    return root_elem;
}

bool is_new_alert(alert_path **acg, alert a){

	alert_path* temp = *acg;

	//Check which paths must be deleted

	while(temp != NULL){ 

		alert_path* path = temp; // path Si

		while(path != NULL){

			if(path->next == NULL && a.seconds_elapsed - path->seconds_elapsed > THRESHOLD){
					
				delete_acg_path(acg, temp);	
				
			}

			path = path->next;
		}
		
		temp = temp->next_path;
	}

	//Check if is new alert

	temp = *acg;
	
	while(temp != NULL){

		alert_path* path = temp; // path Si

		while(path != NULL){

			if(strcmp(a.src_hostname, path->src_hostname) == 0 && strcmp(a.dst_hostname, path->dst_hostname) == 0){
	
				return false;

			}
				
			path = path->next;
		}
		
		temp = temp->next_path;

	}
	
	return true;

}

bool map_alert(graph_node* node_list, alert a){

	while(node_list != NULL){

		if(node_list->vc_node){

			if(strcmp(node_list->src_hostname, a.src_hostname) == 0 && strcmp(node_list->dst_hostname, a.dst_hostname) == 0)
				return true;

		}

		node_list = node_list->next;

	}

	return false;

}

bool update_acg(alert_path** acg, char src_node[], char dst_node[], alert a){ // append a to Si or create new path Si+1

	alert_path* temp = *acg;
	
	while(temp != NULL){

		alert_path* path = temp; // path Si

		while(path != NULL){

			if(strcmp(src_node, path->src_hostname) == 0 && strcmp(dst_node, path->dst_hostname) == 0 
				&& path->next == NULL){ // a is the last element in Si
				
				if(a.seconds_elapsed - path->seconds_elapsed <= THRESHOLD){
					
					insert_alert_acg(&path, a);	
					return true;
					
				}else { return false; }

			}else if (strcmp(src_node, path->src_hostname) == 0 && strcmp(dst_node, path->dst_hostname) == 0 
				&& path->next != NULL){ // if a is not last element in Si than create new path
				
				if(a.seconds_elapsed - path->seconds_elapsed <= THRESHOLD){

					alert_path* root_new_path = duplicate_path(temp, path); //check here
					root_new_path->next_path = *acg;
					*acg = root_new_path;
					
					insert_alert_acg(&path, a);
			
					return true;
					
				}else { return false; }
	
			}
					
			
			path = path->next;
		}
		
		temp = temp->next_path;

	}
	
	return false;

}

bool alert_correlation(graph_node** sag, alert_path** acg, alert a){

	//if ip is outside network

	/*if (alert_outside_network(a) > 0){

		memset(a.src_hostname, 0, strlen(a.src_hostname)*sizeof(char));
		strcpy(a.src_hostname, "internet");
		a.src_hostname[strlen(a.src_hostname)*sizeof(char)] = 0;

	}*/

	if(is_new_alert(acg, a)){ printf("\nNew alert\n");
		
		if(!map_alert(*sag, a)) // New alert that could not be mapped
			return false; 

		//if alert mapped and src is internet than return true

		//New alert and acg is empty

		if(strcmp(a.src_hostname, "internet") == 0){

			//Create new path
				
			alert_path* temp = (alert_path*)malloc(sizeof(alert_path));

			strcpy(temp->src_hostname, a.src_hostname);
			strcpy(temp->dst_hostname, a.dst_hostname);

			temp->seconds_elapsed = a.seconds_elapsed;

			temp->next = NULL;
	
			temp->next_path = *acg;
			*acg = temp;
								
			return true;

			} 
			
		graph_node* temp = *sag;

		while(temp != NULL){ 

			if(temp->vc_node){

				if(temp->parent_list && strcmp(temp->src_hostname, a.src_hostname) == 0 && 
					strcmp(temp->dst_hostname, a.dst_hostname) == 0){ //temp -> node n1
					
					graph_node* parents_n1 = temp->parent_list; //n1 parents
	
					while(parents_n1 != NULL){ //for all n1's parents
							
						//append a to Si or create new path Si+1
						if(update_acg(acg, parents_n1->src_hostname, parents_n1->dst_hostname, a)){ 
							
							return true;
						}

						parents_n1 = parents_n1->next;
					
					}				

				}

			}
		
			temp = temp->next;
		}
		
		return false;

	} 

	else{ //alert is not new 
		
		alert_path* temp = *acg;
		
		while(temp != NULL){

			alert_path* path = temp;
			
			while(path != NULL){

				if(strcmp(path->src_hostname, a.src_hostname) == 0 && strcmp(path->dst_hostname, a.dst_hostname) == 0
					&& a.seconds_elapsed - path->seconds_elapsed <= THRESHOLD)
					return true;
					
				path = path->next;

			}
	
			temp = temp->next_path;

		}

		return false;

	}

}

void set_vc_hosts(graph_node** vc_node, graph_node* leaf_node){

	if(strstr(leaf_node->node_name, "hacl") != NULL){ // hacl

		memset((*vc_node)->src_hostname, 0, sizeof((*vc_node)->src_hostname));
		
		memset((*vc_node)->dst_hostname, 0, sizeof((*vc_node)->dst_hostname));

		compare_strings((*vc_node)->src_hostname, leaf_node->node_name, "(", ",");

		char* sep;
		sep = leaf_node->node_name;

		int comma_num = 0;
		int i = 0;

		while(comma_num < 2){

			if(*sep == ',')
				comma_num++;

			sep++;

			if(comma_num > 0 && comma_num < 2){
			
				if(*sep != ','){
					
					(*vc_node)->dst_hostname[i] = *sep;
					i++;				
				}			

			}
				

		}

		(*vc_node)->dst_hostname[i] = 0;

		remove_char((*vc_node)->src_hostname); 
		remove_char((*vc_node)->dst_hostname); 	
		
	} 
	
	else{

		if(strstr(leaf_node->node_name, "attackerLocated") != NULL){

			memset((*vc_node)->src_hostname, 0, sizeof((*vc_node)->src_hostname));

			compare_strings((*vc_node)->src_hostname, leaf_node->node_name, "(", ")");
			remove_char((*vc_node)->src_hostname); 

		}else{

			memset((*vc_node)->src_hostname, 0, sizeof((*vc_node)->src_hostname));

			compare_strings((*vc_node)->src_hostname, leaf_node->node_name, "(", ",");
			remove_char((*vc_node)->src_hostname);

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

		if(temp->leaf_node){

			while(temp2 != NULL){

				graph_edge* edge_aux = temp->edge; //--HERE

				while(temp->edge != NULL){

					if(temp2->vc_node && temp2->id == temp->edge->dst_id){
										
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

		if(temp->vd_node){

			while(temp2 != NULL){

				graph_edge* edge_aux = temp->edge; //--HERE

				while(temp->edge != NULL){

					if(temp2->vc_node && temp2->id == temp->edge->dst_id){

						if(strstr(temp2->src_hostname, "EMPTY") != NULL) //There is "EMPTY" string
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

		if(temp->vc_node && strstr(temp->dst_hostname, "EMPTY") != NULL){

			memset(temp->dst_hostname, 0, sizeof(temp->dst_hostname));
			strcpy(temp->dst_hostname, temp->src_hostname);

		}

		temp = temp->next;

	}

	//Set vc nodes parents-----------------------------------

	temp = *sag;

	while(temp != NULL){

		if(temp->vc_node && temp->edge != NULL){ //temp -> vc_node
		
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

#endif
