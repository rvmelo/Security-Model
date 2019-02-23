#ifndef TCP_SOCKET
#define TCP_SOCKET


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
#include "SocketTCP.h" 

#define TRUE   1 
#define FALSE  0 
#define PORT 9005 
#define SIZE 19

int opt = TRUE;  
int master_socket , addrlen , new_socket , client_socket[30] , max_clients = 30 , activity, valread , sd;  
int max_sd;  

struct sockaddr_in address;

//set of socket descriptors 
fd_set readfds;  

void write_patterns(int fd, cells* array, int array_size, int sz_emit)
 {
   //#######################
   //    server code
   //#######################
   int i=0, sz=0;
   for(i = 0; i < array_size; i += sz_emit)
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
   
    //initialise all client_socket[] to 0 so not checked 
    for (int i = 0; i < max_clients; i++)  
    {  
        client_socket[i] = 0;  
    }  
        
    //create a master socket 
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)  
    {  
        perror("socket failed");  
        exit(EXIT_FAILURE);  
    }

    int value = 1;
    setsockopt( master_socket, IPPROTO_TCP, TCP_NODELAY, (void *)&value, sizeof(value));
    setsockopt( master_socket, IPPROTO_TCP, TCP_QUICKACK, (void *)&value, sizeof(value));  
    
    //set master socket to allow multiple connections , 
    //this is just a good habit, it will work without this 
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
          sizeof(opt)) < 0 )  
    {  
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }  
    
    //type of socket created 
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;  
    //inet_aton("192.168.25.24", &address.sin_addr);
    address.sin_port = htons( PORT );  
        
    //bind the socket to localhost port 8888 
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)  
    {  
        perror("bind failed");  
        exit(EXIT_FAILURE);  
    }  
    printf("Listener on port %d \n", PORT);  
        
    //try to specify maximum of 3 pending connections for the master socket 
    if (listen(master_socket, 3) < 0)  
    {  
        perror("listen");  
        exit(EXIT_FAILURE);  
    }  
        
    //accept the incoming connection 
    addrlen = sizeof(address);  
    puts("Waiting for connections ...");  


}

void check_incoming_connections(int vm_number){

char *message = "ECHO Daemon v1.0 \r\n"; 
int vm_amount = 0;

  printf("client sockets: ");
  for(int i = 0; i<max_clients; i++)
	printf("%d ", client_socket[i]);printf("\n");

  while(TRUE)  
  { 

       //check number of vms connected

        for(int i = 0; i<max_clients; i++){

           if(client_socket[i] != 0)
		vm_amount++;           


        }

        if(vm_amount >= vm_number)
	    break;
        else
            vm_amount = 0; 
 
        //clear the socket set 
        FD_ZERO(&readfds);  
    
        //add master socket to set 
        FD_SET(master_socket, &readfds);  
        max_sd = master_socket;  
            
        //add child sockets to set 
        for ( int i = 0 ; i < max_clients ; i++)  
        {  
            //socket descriptor 
            sd = client_socket[i];  
                
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);  
                
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }  
    
        //wait for an activity on one of the sockets , timeout is NULL , 
        //so wait indefinitely 
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
      
        if ((activity < 0) && (errno!=EINTR))  
        {  
            printf("select error");  
        }  
            
        //If something happened on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(master_socket, &readfds))  
        {  
            if ((new_socket = accept(master_socket, 
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)  
            {  
                perror("accept");  
                exit(EXIT_FAILURE);  
            }

            //int value2 = 1;
            //setsockopt( new_socket, IPPROTO_TCP, TCP_NODELAY, (void *)&value2, sizeof(value2));
            //setsockopt( new_socket, IPPROTO_TCP, TCP_QUICKACK, (void *)&value2, sizeof(value2));  
            
            //inform user of socket number - used in send and receive commands 
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs
                  (address.sin_port));  
          
            /*send new connection greeting message 
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )  
            {  
                perror("send");  
            }  
                
            puts("Welcome message sent successfully");*/  
                
            //add new socket to array of sockets 
            for (int i = 0; i < max_clients; i++)  
            {  
                //if position is empty 
                if( client_socket[i] == 0 )  
                {  
                    client_socket[i] = new_socket;  
                    printf("Adding to list of sockets as %d\n" , i);  
                        
                    break;  
                }  
            }  
        }
  }

printf("client sockets: ");
  for(int i = 0; i<max_clients; i++)
	printf("%d ", client_socket[i]);printf("\n");
 
}

void check_IO_operation(int vm_number, int destination_vm_index){

  double data[SIZE] = {0.0};

  struct Node* memory_cells = NULL;
  struct Node* temp = NULL;
  struct Node* aux = NULL; 

  while(TRUE)  
     {  
            
        //else its some IO operation on some other socket to receive memory cells from client
        for (int i = 0; i < vm_number;)  
        {
               
            if(i == destination_vm_index){

               i++;
               continue;
            
            }  
            
            sd = client_socket[i];  
                
            if (FD_ISSET( sd , &readfds))  
            { 
		             
                //Check if it was for closing , and also read the 
                //incoming message 
                if (((valread = recv( sd , &data, 19*sizeof(double), 0)) == 0))  
		//if ((recv(sd, &memory_cells, list_size(memory_cells)*sizeof(struct Node), 0)) == 0)  
                {  
                    //Somebody disconnected , get his details and print 
                    getpeername(sd , (struct sockaddr*)&address , \
                        (socklen_t*)&addrlen);  
                    printf("Host disconnected , ip %s , port %d \n" , 
                          inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
                        
                    //Close the socket and mark as 0 in list for reuse 
                    close( sd );  
                    client_socket[i] = 0;  
                }  
                    
                //Echo back the message that came in 
                else
                {
                    printf("valread value: %d\n", valread);
                    
                    if(valread != -1){
                        
                        for(int k = 0; k<19; k++)
			        printf("%.2lf ", data[i]);
                        printf("\n");

                     if(data[0] != -1){ 

                        //insert in memory_cells list

                        insert_list(&aux, data);

                      } else{ //if socket has no memory cells to send anymore

                         //remove extra memory cells (antigens)
        
                           temp = aux;
                           struct Node* temp2 = NULL;
                           int counter = 0;

                           while(aux != NULL || counter < 4){
        
                              aux = aux->next;
                              counter++;
       
                            }

                           temp2 = aux->next;

                           aux->next = NULL;

                           aux = temp;

                           while(temp2 != NULL){
    
                              temp2 = temp2->next;
                              free(temp2);
 
                             }

                           while(aux != NULL){
                                     
                              insert_list(&memory_cells, aux->pattern);
                              aux = aux->next;   

                            }

                           aux = temp;
                           delete_list(&aux); 
                           i++;

                         
                        }  
                        
                      }
                    
                }  
            }  
        }

        
       //send memory_cells to destination vm
       
       int bytes_sent = 0;

       while(memory_cells != NULL){
       
         while((bytes_sent = send(client_socket[destination_vm_index], memory_cells->pattern, SIZE*sizeof(double), 0)) <= 0){

            if(bytes_sent > 0)
		break;

         }

       memory_cells = memory_cells->next;
       bytes_sent = 0;


       }

       // to make client stop receiving patterns
       temp->pattern[0] = -1;
       send(client_socket[destination_vm_index], temp->pattern, SIZE*sizeof(double), 0);
       
 	
       break;
  
    }        

}

void send_message(int destination_vm_index){

   int bytes_sent = 0;

   int send_value = 1; //1 if VM sends memory cells 0 otherwise
   
          // VMs that will send memory cells  

          for(int i = 0; i < max_clients; i++){
           
          	if(client_socket[i] != client_socket[destination_vm_index] && client_socket[i] != 0){

             		while(bytes_sent != sizeof(int)){

                 		send_value = 1; 
            
                 		bytes_sent = send(client_socket[i], &send_value, sizeof(int), 0);
                 		//printf("bytes_sent: %d\n", bytes_sent); 
             		} 
              
           	}
	   }

           bytes_sent = 0;

           // VM that will receive memory cells (only one)

           	while(bytes_sent != sizeof(int)){

                    send_value = 0; 

                    bytes_sent = send(client_socket[destination_vm_index], &send_value, sizeof(int), 0);
                    //printf("bytes_sent 2: %d\n", bytes_sent);
                        
                }
     
}

void orchestra(int vm_number){
 
  const int recv_cells = 15;

  double pattern[SIZE];
  cells data;
  
  for(int i = 0; i<SIZE; i++)
	data.pattern[i] = -1;
  data.confirm = 0;

  int length = 0; int read_err = 0;

  struct Node* memory_cells = NULL; 

  for(int i = 0; i < vm_number; i++){

      send_message(i); // i -> destination vm

      //receive memory cells from vms

      for(int k = 0; k < vm_number; k++){

         int bytes_recv = 0;

         printf("\nrecv 1\n");

         if(k != i){
	    for(int c = 0; c < recv_cells; c++){
                
                bytes_recv = recv(client_socket[k], &data, sizeof(cells), 0);
                insert_list(&memory_cells, data.pattern);
                memory_cells-> confirm = data.confirm;
            }
         }
      
         printf("\nrecv 2\n");

         //printf("size of: %d size recv: %d\n", (int)(sizeof(cells)*recv_cells), bytes_recv);          
              
      }

      sort_confirm(&memory_cells);
      printf("received list: \n");
      print_list(memory_cells);

      //send memory cells to destination vm

      printf("\nsend 1\n");

      while(memory_cells != NULL){

          for(int k =0; k<recv_cells; k++)
		data.pattern[k] = memory_cells->pattern[k];
          data.confirm = memory_cells->confirm; 
          
          send(client_socket[i], &data, sizeof(cells), 0);

          memory_cells = memory_cells->next;
      }

        printf("\nsend 2\n"); 

      //write_patterns(client_socket[i], data, recv_cells*vm_number, (int)sizeof(cells)); 

      //check_IO_operation(vm_number, i); 

   }

  

}

#endif
