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

#define TRUE   1 
#define FALSE  0 
#define PORT 9005 
#define MAX_DETECTORS 300

int main()
{

  srand ( time(NULL) );

  const int recv_cells = 5; const int vm_number = 2;

  //Orchestra Management
 
  start_connection();

  check_incoming_connections(vm_number); //number of vms

  orchestra(vm_number, recv_cells); //number of vms
  
  return 0;
}
