#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>

#include "graph.h"
#include "sag.h"

#define THRESHOLD 2

memory_ids* m_ids = NULL;

void insert_memory_ids(memory_ids** list, int id){

	memory_ids* temp = (memory_ids*)malloc(sizeof(memory_ids));

	temp->id = id;
	
	temp->next = *list;
	*list = temp;

}

void insert_connection_id(id_data** list, int id){

	id_data* temp = (id_data*)malloc(sizeof(id_data));

	temp->id = id;
	temp->full = false;
	
	temp->next = *list;
	*list = temp;

}

void insert_alert_acg(alert_path **Si, alert a){

	if(a.mapped_position->src_sag_node->vd_node)
		return;

	alert_path* temp = (alert_path*)malloc(sizeof(alert_path));

	temp->src_id = a.mapped_position->src_sag_node->id;
	temp->dst_id = a.mapped_position->dst_sag_node->id; 

        //IDs to connect

	graph_edge* edge_aux = a.mapped_position->dst_sag_node->edge;

	while(edge_aux != NULL){

		insert_connection_id(&(temp->connection_ids), edge_aux->node->id);
		edge_aux = edge_aux->next;
		
	}

	//--IDs to connect

	//Used IDs

	memory_ids* memory_aux = m_ids;
	id_data* ids_aux = temp->connection_ids;

	while(memory_aux != NULL){
		
		while(temp->connection_ids != NULL){

			if(temp->connection_ids->id == memory_aux->id)
				temp->connection_ids->full = true;

			temp->connection_ids = temp->connection_ids->next;

		}
		
		temp->connection_ids = ids_aux;
		memory_aux = memory_aux->next;
	
	}

	//--Used IDs

	temp->alert_ts = a.alert_ts;

	temp->next_path = NULL;

	if(*Si == NULL){

		temp->next = NULL;
		*Si = temp;

	}else{

		alert_path* temp2 = *Si;

		while(1){ //printf("\nloop\n");

			if((*Si)->next == NULL){

				//printf("\nloop\n");
				
				temp->next = NULL;
				(*Si)->next = temp;
				*Si = temp2;	
				break;

			}
				
			*Si = (*Si)->next;

		}

	}

}

void insert_path_acg(alert_path** acg, alert a){ //First insertion of a new path

	if(a.mapped_position->src_sag_node->vd_node)
		return;

	alert_path* temp = (alert_path*)malloc(sizeof(alert_path));

	temp->src_id = a.mapped_position->src_sag_node->id;
	temp->dst_id = a.mapped_position->dst_sag_node->id; 

	 //IDs to connect

	graph_edge* edge_aux = a.mapped_position->dst_sag_node->edge;

	while(edge_aux != NULL){

		insert_connection_id(&(temp->connection_ids), edge_aux->node->id);
		edge_aux = edge_aux->next;
		
	}


	//--IDs to connect

	//Used IDs

	memory_ids* memory_aux = m_ids;
	id_data* ids_aux = temp->connection_ids;

	while(memory_aux != NULL){
		
		while(temp->connection_ids != NULL){

			if(temp->connection_ids->id == memory_aux->id)
				temp->connection_ids->full = true;

			temp->connection_ids = temp->connection_ids->next;

		}
		
		temp->connection_ids = ids_aux;
		memory_aux = memory_aux->next;
	
	}

	//--Used IDs

	temp->alert_ts = a.alert_ts;

	temp->next = NULL;
	
	temp->next_path = *acg;
	*acg = temp;

}

void print_acg(alert_path* acg){

	int i = 0;

	while(acg != NULL){

		i++;
		
		alert_path* path_aux = acg;

		printf("Path %d:\n\n", i);

		while(path_aux != NULL){

			printf("src id: %d ", path_aux->src_id);
			printf("dst id: %d\n", path_aux->dst_id);
			
			path_aux = path_aux->next;

		}

		printf("\n");
                
		acg = acg->next_path;

	}

}

alert_path *duplicate_path(alert_path *path, alert_path *path_aux)
{
     alert_path*  path_2 = 0;
     alert_path* preElem = 0;

     // Move through all the nodes in source list.
     while(path != 0) {

          // Create the memory for the destination list node.
          alert_path *elem = (alert_path*)malloc (sizeof(alert_path));

          if(elem == 0) {
               printf("Failed allocation!"), exit(1);
          }

          // Copy the data from the source to dest node
	  elem->src_id = path->src_id;
	  elem->dst_id = path->dst_id; 

	  elem->alert_ts = path->alert_ts;
	  elem->next = 0;

          if (path_2 == 0) {
                // keep the root for the destination list.
                path_2 = elem;
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

    return path_2;
}

bool is_new_alert(alert_path* acg, alert a){

	if(acg == NULL)
		return true;

	alert_path* temp = acg;

	bool new_node = true;

	//alert src node is a start node

	while(acg != NULL){
	
		while(a.mapped_position != NULL){

			if(a.mapped_position->src_sag_node->start_node && a.mapped_position->src_sag_node->id == acg->src_id && 				difftime(a.alert_ts, acg->alert_ts) < THRESHOLD)
				new_node = false;

			a.mapped_position = a.mapped_position->next;

		}

		acg = acg->next_path;

	}

	//--alert src node is a start node

	acg = temp;

	while(acg != NULL){

		alert_path* path_aux = acg;
	
		while(acg != NULL){

			mapped_sag* map_aux = a.mapped_position;

			while(a.mapped_position != NULL){

				if(a.mapped_position->src_sag_node->id == acg->src_id 
				&& a.mapped_position->dst_sag_node->id == acg->dst_id			 
				&& difftime(a.alert_ts, temp->alert_ts) < THRESHOLD
				&& !a.mapped_position->src_sag_node->start_node)
					new_node = false;

				a.mapped_position = a.mapped_position->next;

			}

			a.mapped_position = map_aux;
			acg = acg->next;

		}

		acg = path_aux;
		acg = acg->next_path;

	}

	
	return new_node;
	
}

void map_alert(graph_node** sag, alert* a, char src[], char dst[]){

	if((*sag) == NULL){
		return;
	}
	
	else{

		graph_edge* temp = (*sag)->edge;

		while((*sag)->edge != NULL){

			if((*sag)->edge->path == false){

				(*sag)->edge->path = true;

				if((*sag)->vd_node || strstr((*sag)->node_name, "attackerLocated") != NULL){
					
					memset(src, 0, strlen(src)*sizeof(char));					
					strcpy(src, (*sag)->hostname);
					src[strlen(src)*sizeof(char)] = 0;

					if(strstr((*sag)->node_name, "attackerLocated") != NULL)
						(*sag)->edge->node->start_node = true;	
				}

				if((*sag)->edge->node->vd_node){

					memset(dst, 0, strlen(dst)*sizeof(char));			
					strcpy(dst, (*sag)->edge->node->hostname); 
					dst[strlen(dst)*sizeof(char)] = 0;	
				}else{

					memset(dst, 0, strlen(dst)*sizeof(char));			
					strcpy(dst, src); 
					dst[strlen(dst)*sizeof(char)] = 0;
				}

				if(strcmp(src, (*a).alert_src_ip) == 0 && strcmp(dst, (*a).alert_dst_ip) == 0){
										
					insert_mapped_position(&((*a).mapped_position), *sag, (*sag)->edge->node);

				}
				
				map_alert(&((*sag)->edge->node), a, src, dst);
			}

			if((*sag)->edge != NULL)

				(*sag)->edge = (*sag)->edge->next;

			else break; 

	        }

		(*sag)->edge = temp;

	}		

}

bool match_alert(id_data** path_ids, graph_node* node){

	id_data* temp = *path_ids;

	while(*path_ids != NULL){
		
		if((*path_ids)->id == node->id && !(*path_ids)->full){					
				
			printf("\npath true id: %d \n", (*path_ids)->id);
			
			(*path_ids)->full = true;
			*path_ids = temp;

			insert_memory_ids(&m_ids, (*path_ids)->id);			
			
			return true;
		}
		
		*path_ids = (*path_ids)->next;
	}
	
	*path_ids = temp;
	
	return false;
	
}

bool create_node_acg(alert_path** acg, alert* a){

	bool insert_value = false;

	alert_path* temp = *acg;

	if(*acg == NULL){ //First alert

		printf("\nPART 1\n");

		mapped_sag* map_aux = a->mapped_position;

		while(a->mapped_position != NULL){ 
	 
			if(a->mapped_position->src_sag_node->start_node){	

				insert_value = true;				
				insert_path_acg(acg, *a);
						
			}

			a->mapped_position = a->mapped_position->next;

		}

		a->mapped_position = map_aux;

		return insert_value;

	}	


	while(*acg != NULL){

		alert_path* path_aux = *acg;
		
		while(path_aux != NULL){

			bool insert_path = false;			
			mapped_sag* map_aux = a->mapped_position;

			printf("path_aux src id: %d ", path_aux->src_id);
			printf("path_aux dst id: %d \n", path_aux->dst_id);
			
			while(a->mapped_position != NULL){

				if(a->mapped_position->src_sag_node->start_node){

					insert_path_acg(acg, *a); 
					
					insert_value = true; insert_path = true;
	
					//break;

				}			
				else if((a->mapped_position) && match_alert(&(path_aux->connection_ids),a->mapped_position->src_sag_node) 
				&& difftime(a->alert_ts, path_aux->alert_ts) < THRESHOLD){

					insert_value = true; insert_path = true; 

					if(path_aux->next != NULL){ //Duplicate path

						alert_path* aux = duplicate_path(*acg, path_aux);
						aux->next_path = temp; 
						temp = aux;

						insert_alert_acg(&temp, *a);
					 		

					} else insert_alert_acg(acg, *a); 
													
				}
											
					a->mapped_position = a->mapped_position->next;

			}
					
			a->mapped_position = map_aux;
			path_aux = path_aux->next; 
			
		}
		
		*acg = (*acg)->next_path;
	}

	*acg = temp;

	return insert_value;	

}

void alert_correlation(alert a, graph_node* sag, alert_path* acg){

	if(is_new_alert(acg, a)){

		

		

	}

}

int main()
{
	
	graph_node* graph = NULL; 
	
	generate_nodes(&graph, "AttackGraph.txt");
	generate_edges(&graph, "AttackGraph.txt");
	
	printf("\n");

	graph_node* temp = graph; //AQUI!!!!

	while(temp != NULL){

		if(strstr(temp->node_name, "attackerLocated"))		
			break;
		else
			temp = temp->next;

	}

	if(temp == NULL){
		
		printf("Error... attacker node not found\n");	
		return 0;

	}

	alert_path* acg = NULL; 
	alert a  = { .mapped_position = NULL };
	alert a2  = { .mapped_position = NULL };

	strcpy(a.alert_src_ip, "internet");
	strcpy(a.alert_dst_ip, "webServer");

	char src[100] = "whatever I need"; 
	char dst[100] = "whatever you need?";

	map_alert(&temp, &a, src, dst);

	printf("\nprint acg:\n\n");

	create_node_acg(&acg, &a);

	print_acg(acg);

	strcpy(a2.alert_src_ip, "webServer");
	strcpy(a2.alert_dst_ip, "webServer");

	for(int i = 0; i<5;i++){

		if(i == 4){

			memset(a2.alert_src_ip, 0, strlen(a2.alert_src_ip)*sizeof(char));	
			memset(a2.alert_dst_ip, 0, strlen(a2.alert_dst_ip)*sizeof(char));
 			
			strcpy(a2.alert_src_ip, "webServer");
			strcpy(a2.alert_dst_ip, "fileServer");
		}

		initialize_graph_path(&graph);
	
		char src2[100] = "whatever I need"; 
		char dst2[100] = "whatever you need?";

		a2.mapped_position = NULL;

		map_alert(&temp, &a2, src2, dst2);

		if(is_new_alert(acg, a2))
			create_node_acg(&acg, &a2);

	}

	printf("\nprint acg 2:\n\n");
	print_acg(acg);

	
	return 0;
	
}
