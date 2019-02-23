#ifndef SAG
#define SAG

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include "graph.h"

void count(char *file_name, int counter){

	FILE* fptr = fopen(file_name, "r");

	int c; 
	while ( (c = fgetc (fptr)) != EOF )
		if ( c == '\n' )
    			counter++;

	printf("The file has %d lines", counter);

}

void compare_strings(char name[], char node_name[], const char* symbol_1, const char* symbol_2){

	char *sep; 

	strtol(node_name, &sep, 0);
	sep--;
	
	int position = strcspn(node_name, symbol_1)+1;
	int position2 = strcspn(node_name, symbol_2);

	strncpy(name, node_name + position, (node_name + position2) - (node_name + position));	

}

void insert_node(graph_node** node, graph_node data){

	graph_node* temp = (graph_node*)malloc(sizeof(graph_node));
	
	temp->id = data.id;
		
	strcpy(temp->node_name, data.node_name);
	temp->node_name[sizeof(temp->node_name) - 1] = 0;

	strcpy(temp->hostname, data.hostname);
	temp->hostname[sizeof(temp->hostname) - 1] = 0;

	temp -> vd_node = data.vd_node;

	temp -> edge = NULL;
		 								
	temp -> next = *node;
	*node = temp;
			
	}

void insert_mapped_position(mapped_sag** mapped_position, graph_node* src, graph_node* dst){

	if(src && src->vd_node)
		return;
	
	mapped_sag* temp = (mapped_sag*)malloc(sizeof(mapped_sag));	

	temp->src_sag_node = src;
	temp->dst_sag_node = dst;

	temp->next = *mapped_position;
	*mapped_position = temp; 

}

void insert_edge(graph_edge** edge, graph_node** node, int src, int dst){

	graph_edge* temp = (graph_edge*)malloc(sizeof(graph_edge));

	temp->src_id = src; 
	temp->dst_id = dst;

	(*node)->edge_number++;

	temp->node = *node;

	temp->path = false;

	temp->next = *edge;
	*edge = temp;
}

void print_nodes(graph_node* graph){
	
	graph_node* temp = graph;
		
	while (temp != NULL){
						
		printf("%d ", temp->id);

		//puts(temp->node_name);

		printf("%s ", temp->node_name);

		printf(" vd_node: %d", temp->vd_node);

		printf(" edge number: %d", temp->edge_number);

		printf(" alerts mapped: %d", temp->alerts_mapped);

		//printf(" hostname: %s", temp->hostname);
				
		printf("\n");printf("\n");
		
		temp = temp->next;
			
	}	
	
}

void initialize_graph_path(graph_node** graph){

	graph_node* temp = *graph;

	while(*graph != NULL){

		graph_edge* temp2 = (*graph)->edge;

		while((*graph)->edge != NULL){

			(*graph)->edge->path = false;
	
			(*graph)->edge = (*graph)->edge->next;

		}

		(*graph)->edge = temp2;

		*graph = (*graph)->next;

	}

	*graph = temp;

}

void print_graph(graph_node** graph){

	if((*graph) == NULL){
		return;
	}
	
	else{
		
		printf("%d ", (*graph)->id);
		puts((*graph)->node_name);				
		printf("\n");

		graph_edge* temp = (*graph)->edge;

		while((*graph)->edge != NULL){

			if((*graph)->edge->path == false){

				(*graph)->edge->path = true;
				
				print_graph(&((*graph)->edge->node));			
			}

			if((*graph)->edge != NULL){

				//(*graph)->edge->path = false;
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

		if((*graph)->id == src){ //search for edge source
			
			while(temp2 != NULL){ 

				if(temp2->id == dst){ //search for edge destination

					insert_edge(&((*graph)->edge), &temp2, src, dst);					
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
    struct Graph_Node data = { .next = NULL, .edge = NULL };

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

        if(strlen(line) <= 12)
            continue; // resume reading, skip the line

	if(strstr(line, "OR") != NULL)
		data.vd_node = true;
	else
		data.vd_node = false;

        char *sep;
        long int id = strtol(line, &sep, 0);

        // assuming that there is not white spaces
        // before and after the commas
        if(*sep != ',')
        {
            fprintf(stderr, "Warning, line %lu is malformatted, '<id>,' exepcted\n", linenmr);
            continue;
        }

        data.id = id;

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

        strncpy(data.node_name, sep + 2, sizeof data.node_name);
        data.node_name[sizeof(data.node_name) - 1] = 0;

	if(data.vd_node)
        	compare_strings(data.hostname, data.node_name, "(", ",");
	else if(strstr(data.node_name, "attackerLocated"))
		compare_strings(data.hostname, data.node_name, "(", ")");

        insert_node(graph, data);

	memset(data.node_name, 0, sizeof(data.node_name));
	memset(data.hostname, 0, sizeof(data.hostname));

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

		if(strlen(line) > 12)
	            	continue; // resume reading, skip the line

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

#endif	

