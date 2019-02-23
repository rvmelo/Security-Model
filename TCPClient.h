/****************** CLIENT CODE ****************/

#ifndef TCP_CLIENT
#define TCP_CLIENT

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
#include "TCPCommunication.h"

#define TRUE 1

int clientSocket;
int recv_value = -1;

socklen_t addr_size;
struct sockaddr_in serverAddr;

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


void close_client(){

   close(clientSocket);

}

void client(struct Node* memory_cells, struct Node** detectors, int max_cells_to_send, int vm_number){

  bool send_bool = false;
  bool recv_bool = false;
  int send_value = 0;
  int bytes_recv = 0;

  int send_count = 0;

  cells data;

  int size = list_size(memory_cells);

  struct Node* temp = memory_cells;
  struct Node* next = NULL;

  if(memory_cells == NULL || size == 0)
      printf("WARNING!! NO MEMORY CELLS TO SEND!");

  //reduce memory_cells list to 'max_cells_to_send' size with the highest confirm values

  //printf("PART 1 DEFINE MAX CELLS TO SEND\n\n");
 
  if(size > max_cells_to_send){

      for(int i = 0; i < size - max_cells_to_send; i++){

                next = temp->next;	
		temp = next;     
      }

      memory_cells = temp;    

  }else{

     max_cells_to_send = size;

  }

  //printf("PART 2 DEFINE MAX CELLS TO SEND\n\n");
  
  while(TRUE){

    //printf("PART 3 RECV SEND VALUE\n\n");

    receive_int(&send_value, clientSocket); 

    //SEND MEMORY CELLS

    //printf("PART 4 RECV SEND VALUE\n\n");

    if(send_value == 1 && send_count <= vm_number-1){

       send_count++;

       //printf("PART 5 SEND  MAX CELLS TO SEND\n\n");

       send_int(max_cells_to_send, clientSocket);

       //printf("PART 6 SEND MAX CELLS TO SEND\n\n");

       send_bool = true;
       
       int bytes_sent = 0;

       //printf("PART 7 SEND MEMORY CELLS\n\n");

       while(memory_cells != NULL){ 
           
           for(int i = 0; i < SIZE; i++)
                 data.pattern[i] = memory_cells->pattern[i];
           
           data.confirm = memory_cells->confirm; 
                        
           send_data(data, clientSocket);
           
           memory_cells = memory_cells->next;

       } 

       //printf("PART 8 SEND MEMORY CELLS\n\n");

       memory_cells = temp; //it makes memory_cells point to the first element on list so memory_cells list can be sent again
     
    }


    //RECEIVE MEMORY CELLS
    
    if(!recv_bool && send_value == 0){

        int cells_to_recv = 0;

        //printf("PART 9 CELLS TO RECV\n\n");

        receive_int(&cells_to_recv, clientSocket);

        //printf("PART 10 CELLS TO RECV\n\n");

        recv_bool = true; 

        for(int i = 0; i<cells_to_recv; i++){

            recv_data(&data, clientSocket);
            insert_list(detectors, data.pattern);
            (*detectors)->confirm = data.confirm;

        }

        //printf("PART 11 RECV MEMORY CELLS\n\n"); 
 
   }

    if(recv_bool && send_count >= vm_number-1) break;

  }
  
  sort_confirm(detectors);

  //printf("PART 12 OUTSIDE LOOP\n\n"); 

}

#endif
