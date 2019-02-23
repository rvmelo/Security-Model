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
#include "TCPServer.h"
#include "TCPCommunication.h"

#define TRUE   1 
#define FALSE  0 
#define PORT 9005 
#define MAX_DETECTORS 300

int main()
{

  srand ( time(NULL) );

  const int max_detectors = 150; const int vm_number = 2;

  //Orchestra Management
 
  start_connection();

  check_incoming_connections(vm_number); //number of vms

  for(int i = 0; i<ITERATIONS; i++){
  	orchestra(vm_number, max_detectors);
        printf("iteration: %d\n", i); 
  } 
  
  
  return 0;
}
