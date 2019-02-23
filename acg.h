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

void delete_acg(alert_path** acg_head_ref){

	/* deref acg_head_ref_ref to get the real head */
	alert_path* current = *acg_head_ref;

	while (current != NULL) 
   	{
       		alert_path* temp = current;
		delete_acg_path(&current, temp);		    
   	}
   
   	/* deref head_ref to affect the real head back
      	in the caller. */
   	*acg_head_ref = NULL;

}


int alert_outside_network(alert a){

	//new amazon web service network - class - DoS/Pobe/R2L/U2R

	if(strcmp(a.src_hostname, "172.31.18.33") == 0 || strcmp(a.src_hostname, "172.31.17.231") == 0 || 
	strcmp(a.src_hostname, "172.31.26.117") == 0 || strcmp(a.src_hostname, "172.31.31.169") == 0 || 
	strcmp(a.src_hostname, "172.31.17.50") == 0 || strcmp(a.src_hostname, "172.31.20.3") == 0 || 
	strcmp(a.src_hostname, "172.31.17.74") == 0 || strcmp(a.src_hostname, "172.31.29.192") == 0 ||
	strcmp(a.src_hostname, "172.31.24.16") == 0 || strcmp(a.src_hostname, "172.31.17.124") == 0 ||
	strcmp(a.src_hostname, "172.31.27.71") == 0)

	//old amazon web service network - class - DoS/DDoS

	/*if(strcmp(a.src_hostname, "172.31.29.192") == 0 || strcmp(a.src_hostname, "172.31.18.33") == 0 || 
	strcmp(a.src_hostname, "172.31.17.231") == 0 || strcmp(a.src_hostname, "172.31.27.71") == 0 || 
	strcmp(a.src_hostname, "172.31.26.117") == 0 || strcmp(a.src_hostname, "172.31.31.169") == 0 || 
	strcmp(a.src_hostname, "172.31.17.74") == 0 || strcmp(a.src_hostname, "172.31.17.50") == 0 ||
	strcmp(a.src_hostname, "172.31.20.3") == 0)*/

	//older amazon web service network

	/*if(strcmp(a.src_hostname, "172.31.29.192") == 0 || strcmp(a.src_hostname, "172.31.18.33") == 0 || 
	strcmp(a.src_hostname, "172.31.27.3") == 0 || strcmp(a.src_hostname, "172.31.27.71") == 0 || 
	strcmp(a.src_hostname, "172.31.30.80") == 0 || strcmp(a.src_hostname, "172.31.22.208") == 0 || 
	strcmp(a.src_hostname, "172.31.17.74") == 0 || strcmp(a.src_hostname, "172.31.22.238") == 0 ||
	strcmp(a.src_hostname, "172.31.20.3") == 0)*/

	/*if(strcmp(a.src_hostname, "192.168.25.28") == 0 || strcmp(a.src_hostname, "192.168.25.36") == 0)*/

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

void calculate_distance(graph_node** graph, int* min_distance, int distance){


	if((*graph)->edge == NULL){ // when target node is reached
		
		if(*min_distance > distance)
			*min_distance = distance;

		return;

	}

	else{

		graph_edge* temp = (*graph)->edge;

		while((*graph)->edge != NULL){

			if((*graph)->edge->path == false){

				(*graph)->edge->path = true; 
			
				calculate_distance(&((*graph)->edge->node), min_distance, distance + 1);
							
			}

			if((*graph)->edge != NULL){
	
				(*graph)->edge = (*graph)->edge->next;

			}

			else break;

	        }

		(*graph)->edge = temp;

	}
		
}

int distance_to_target(graph_node** node_list, alert a){

	bool node_found = false;

	graph_node* node_aux = *node_list;

	while(node_aux != NULL){ //find in which node alert is mapped to

		if(node_aux->data.vc_node){

			host_data* hosts = node_aux->hosts;

			while(hosts != NULL){

				if(strcmp(hosts->src_hostname, a.src_hostname) == 0 && strcmp(hosts->dst_hostname, a.dst_hostname) == 0){
					
					node_found = true;
					break;
				
				}
				
				hosts = hosts->next;
	
			}

			if(node_found) break;
		
		}

		node_aux = node_aux->next;

	}

	int distance = 0; int min_distance = 100000;

	*node_list = node_aux; 

	//printf("\n\ncalculate distance id: %d\n\n", node_aux->id);

	calculate_distance(&node_aux, &min_distance, distance);

	return min_distance;

}

bool map_alert_to_node(host_data* node_hosts, alert a){

	while(node_hosts != NULL){

		if(strcmp(node_hosts->src_hostname, a.src_hostname) == 0 && strcmp(node_hosts->dst_hostname, a.dst_hostname) == 0)
					return true;

		node_hosts = node_hosts->next;

	}

	return false;

}


bool map_alert(graph_node* node_list, alert a){

	while(node_list != NULL){

		if(node_list->data.vc_node){

			host_data* hosts = node_list->hosts;

			while(hosts != NULL){

				if(strcmp(hosts->src_hostname, a.src_hostname) == 0 && strcmp(hosts->dst_hostname, a.dst_hostname) == 0)
					return true;
				
				hosts = hosts->next;
	
			}
		
		}

		node_list = node_list->next;

	}

	return false;

}

bool update_acg(alert_path** acg, alert a){ // append a to Si or create new path Si+1

	alert_path* temp = *acg;
	
	while(temp != NULL){

		alert_path* path = temp; // path Si

		while(path != NULL){

			if(strcmp(path->dst_hostname, a.src_hostname) == 0 && path->next == NULL){ // a is the last element in Si
				
				if(a.seconds_elapsed - path->seconds_elapsed <= THRESHOLD){
					
					insert_alert_acg(&path, a);	
					return true;
					
				}else { return false; }

				// if a is not last element in Si than create new path
			}else if (strcmp(path->dst_hostname, a.src_hostname) && path->next != NULL){ 
								
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

	if (alert_outside_network(a) > 0){

		memset(a.src_hostname, 0, strlen(a.src_hostname)*sizeof(char));
		strcpy(a.src_hostname, "internet");
		a.src_hostname[strlen(a.src_hostname)*sizeof(char)] = 0;

	}

	if(is_new_alert(acg, a)){ 
		
		if(!map_alert(*sag, a)) // New alert that could not be mapped
			return false; 

		//if alert mapped and src is internet than return true

		if(strcmp(a.src_hostname, "internet") == 0){ //New alert and a new path is added

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

		//append a to Si or create new path Si+1

		if(update_acg(acg, a)) //alerts have a source IP different from internet
			return true;
		else
			return false;
		
	} 

	else{ //alert is not new

		return true;

	} 

}

#endif
