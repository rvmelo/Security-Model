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

#include "graph.h"
#include "acg.h"

graph_node* sag = NULL;
alert_path* acg = NULL; 

int main (){

/*------------------ Test nodes's mapping---------------

	inicialization(&sag, "AttackGraph.txt");
	print_nodes(sag);

-------------------------------------------------------*/

/*------------------Teste nodes's parents----------------

	inicialization(&sag, "AttackGraph.txt");

	graph_node* temp = sag;

	while(temp != NULL){

		if(temp->vc_node){

			printf("%d ", temp->id); printf("node name: %s: \n", temp->node_name);
			
			printf("node src: %s \n", temp->src_hostname);
			printf("node dst: %s \n\n", temp->dst_hostname);
			
			printf("\nnodes's parents: \n\n");

			graph_node* aux_parent = temp->parent_list;

			while(aux_parent != NULL){

				printf("%d ", aux_parent->id); printf("parent name: %s \n", aux_parent->node_name);
				printf("parent src: %s \n", aux_parent->src_hostname);
				printf("parent dst: %s \n\n", aux_parent->dst_hostname);

				aux_parent = aux_parent->next;

			}
	
			printf("END\n\n");	
		
		}

		temp = temp->next;

	}

-------------------------------------------------*/

/*-------------------mapping test---------------------------

	alert a;
	
	strcpy(a.src_hostname, "fileServer");
	strcpy(a.dst_hostname, "webServer");
	
	inicialization(&sag, "AttackGraph.txt");

	if(map_alert(sag, a))
		printf("\n\nalert mapped\n");
	else
		printf("\n\nalert not mapped\n");

--------------------------------------------------------------*/

	alert a;

	inicialization(&sag, "AttackGraph.txt");	

	while(1){

		printf("src: "); fgets(a.src_hostname, sizeof(a.src_hostname), stdin); 
		printf("dest: "); fgets(a.dst_hostname, sizeof(a.dst_hostname), stdin);
		printf("seconds elapsed: "); scanf("%lf", &a.seconds_elapsed);
		getc(stdin);

		// getting rid of the newline
        	a.src_hostname[strcspn(a.src_hostname, "\n")] = 0; a.dst_hostname[strcspn(a.dst_hostname, "\n")] = 0;

		//printf("src a 2: %s\n", a.src_hostname); printf("dst a 2: %s\n\n", a.dst_hostname);
		
		alert_correlation(&sag, &acg, a);

		memset(a.src_hostname, 0, sizeof(a.src_hostname));
		memset(a.dst_hostname, 0, sizeof(a.dst_hostname));

		printf("\n\nACG:\n\n");

		print_acg(acg);

		printf("\npress 'e' to exit or enter to continue: ");

		int c = getchar();

		if(c == 101)
			break;

	}

	
	return 0;

}
