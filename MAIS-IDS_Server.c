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

int main(int argc, char *argv[])
{

  srand ( time(NULL) );

  struct Node* test_dataset = NULL; 

  if(argc < 3){

  	printf("Argument(s) expected.\n");
  	return 0;

  	}
  
   else if(argc > 3){
 
  	printf("Too many arguments supplied.\n");
	return 0;	

	} 

  const int max_detectors = atoi(argv[1]); 
  const int vm_number = atoi(argv[2]);

  alert_list* a_list = NULL;

  for(int system_runs = 0; system_runs < SYSTEM_RUNS; system_runs++){

  	start_connection(); 

  	//Orchestra Management
 
  	check_incoming_connections(vm_number); //number of vms

  	for(int i = 0; i<ITERATIONS; i++){
  		orchestra(vm_number, max_detectors);
        	printf("iteration: %d\n", i);
  	}

	close_server();

  }
  
  return 0;
}
