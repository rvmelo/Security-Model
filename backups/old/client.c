/****************** CLIENT CODE ****************/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include<math.h>
#include "Data.h"

#include "NSA_NSL_KDD.h"
#include "CLONALG_NSL_KDD.h"
#include "aiNet_NSL_KDD.h"
#include "SortingLists.h"


#define TRUE 1
#define SIZE 19

int clientSocket;
int recv_value = -1;

socklen_t addr_size;
struct sockaddr_in serverAddr;

void write_patterns(int fd, cells* array, int array_size, int sz_emit)
 {
   //#######################
   //    server code
   //#######################
   int i=0, sz=0;
   for(i = 0; i < array_size; i += sz_emit )
   {  
       while(sz_emit-sz)
       { 
         sz+=write(fd, array+i+sz, sz_emit-sz);
       }
       sz = 0;
   }
   //return i;
 }

cells* recv_patterns(int fd, int array_size, int sz_received, int* length, int* read_err)
{
  *read_err = 0;
  int i = 0, sz = 0, rt = 0, count=0;
  cells *array = (cells *)malloc(SIZE);
  memset(array, 0, array_size);  
  for (i = 0; i < array_size; i += sz_received)
    {
      while(sz_received-sz)
      { 
        rt = read(fd, array + i + sz, sz_received-sz);
        if(rt==-1)
        {
          *read_err=rt;
          printf("an error occurs\n");
          goto l;
        }
        if(!rt)goto l;
        sz+=rt;
        count += sz;   
      }
      sz = 0;
    }
  l: *length = count;
 return array;
}

void start_connection(){

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  clientSocket = socket(AF_INET, SOCK_STREAM, 0);

  //int value = 1;
  //setsockopt( clientSocket, IPPROTO_TCP, TCP_NODELAY, (void *)&value, sizeof(value));
  //setsockopt( clientSocket, IPPROTO_TCP, TCP_QUICKACK, (void *)&value, sizeof(value));
  
  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(9005);
  /* Set IP address to localhost */
  
  serverAddr.sin_addr.s_addr = INADDR_ANY; 
  //inet_aton("192.168.25.24", &serverAddr.sin_addr);
  //serverAddr.sin_addr.s_addr = inet_ntoa("192.168.25.28");

  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*---- Connect the socket to the server using the address struct ----*/
  addr_size = sizeof serverAddr;
  int connection_status = connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

  while(connection_status == -1){
	printf("Error\n");
        connection_status = connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
	}

}

bool bool_send (){

//wait for signal to send memory cells

  int bytes_recv = 0;

  while((bytes_recv = recv(clientSocket, &recv_value, sizeof(int), 0)) <= 0){

       printf("bytes_recv: %d\n", bytes_recv);
  	
	if(bytes_recv > 0)
		break;

	} printf("bytes_recv: %d value received: %d\n ", bytes_recv, recv_value);

   //if recv_value > 0 send memory cells to orchestra

   if(recv_value > 0) //send
     return true;
   else
     return false; //not send

}

void receive_memory_cells(struct Node** detectors){

 int result = 0;

 double pattern[SIZE];

 for(int i = 0; i<SIZE; i++)
	pattern[i] = 0.0;
       
     while(pattern[0] != -1){

        result = recv(clientSocket, pattern, SIZE*sizeof(double), 0);

        if(pattern[0] != -1)
          insert_list(detectors, pattern);

     }

}

void send_memory_cells(struct Node* memory_cells){

 double pattern[SIZE]; 
 
 for(int i = 0; i<SIZE; i++)
	pattern[i] = -1;
  
  //if recv_value > 0 send memory cells to orchestra

  if(recv_value > 0){
    
    while(memory_cells != NULL){

        int result = send(clientSocket, memory_cells->pattern, SIZE*sizeof(double), 0);

        memory_cells = memory_cells->next;

	}
       
       send(clientSocket, pattern, SIZE*sizeof(double), 0);

  }

 }

void close_client(){

   close(clientSocket);

}

int main(){

  srand ( time(NULL) );

  struct Node* memory_cells = NULL;
  struct Node* detectors = NULL;

  const int cells_size = 15;

  //cells memory_cells;

  cells data;  

  for(int i = 0; i<SIZE; i++)
	data.pattern[i] = -1;
  data.confirm = 0;
 
  double pattern[SIZE];

  int length = 0; int read_err = 0;

  //Generate data to send

  for(int i = 1; i <16; i++){
  
      for(int k = 0; k < SIZE; k++)
           pattern[k] = i;

      insert_list(&memory_cells, pattern);
      memory_cells->confirm = rand() % 50;
  }

  /*int k = 0; 

  while(test != NULL){

     for(int i = 0 ; i<SIZE; i++)
	memory_cells[k].pattern[i] = test->pattern[i];
     
     memory_cells[k].confirm = rand() % 50;

     k++;
      
     test = test->next;

  }*/

  //---Generate data to send

  bool send_bool = false;
  bool recv_bool = false;
  int send_value = 0;
  int bytes_recv = 0;

  start_connection();

  while(TRUE){

    printf("\n\nHERE\n\n");

    bytes_recv = recv(clientSocket, &send_value, sizeof(int), 0);
    //printf("bytes received: %d send_value: %d\n", bytes_recv, send_value);

    if(!send_bool && send_value == 1){
	send_bool = true;
       
       int bytes_sent = 0;

       printf("\nsend 1\n");
 
       while(memory_cells != NULL){ 
           
           for(int i = 0; i < SIZE; i++)
                 data.pattern[i] = memory_cells->pattern[i];
           
           data.confirm = memory_cells->confirm; 
          
               //while(bytes_sent != sizeof(cells))
        	  //bytes_sent = send(clientSocket, &data, sizeof(cells), 0);
            
           if(send(clientSocket, &data, sizeof(cells), 0) < 0)
		 printf("\nfailure to send\n");
           
 
           memory_cells = memory_cells->next;

       }

       printf("\nsend 2\n");
       //write_patterns(clientSocket, memory_cells, cells_size, (int)sizeof(cells)); 
    }

    
    if(!recv_bool && send_value == 0){
	
        recv_bool = true; printf("\nrecv 1\n");
        
        for(int i = 0; i<cells_size; i++){
            
            recv(clientSocket, &data, sizeof(cells), 0);
            insert_list(&detectors, data.pattern);
            detectors->confirm = data.confirm;

        } printf("\nrecv 2\n");

        //data = recv_patterns(clientSocket, cells_size, (int)sizeof(cells), &length, &read_err); 
    }

    
  
   if(send_bool && recv_bool)
	break;

  }

  /*for(int i = 0; i<cells_size; i++){
      insert_list(&detectors, data[i].pattern);
      data[i].confirm = detectors->confirm;
  }*/
      
  sort_confirm(&detectors);

  printf("\n\nreceived memory cells: \n");
  print_list(detectors);

  printf("\n\nFinished");
  
  close_client();

  return 0;
}
