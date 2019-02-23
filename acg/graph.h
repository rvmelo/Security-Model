#ifndef Graph_Structure
#define Graph_Structure

#include <stdbool.h>
#include <time.h>

struct Graph_Node{
	
	int id;
	char node_name[100];
	
	char src_hostname[50];
	char dst_hostname[50]; 

	bool vd_node; //result of exploit

	bool vc_node; // (Hosts, vul, alert) 

	bool leaf_node;

	bool start_node; //where attacker starts

	int edge_number;
	
	//int alerts_mapped; -> Here!!!
	
	struct Graph_Node* next;
	struct Graph_Edge* edge;

	struct Graph_Node* parent_list;

	struct alert_list* alert_set;
	
	};

struct Graph_Edge{

	int src_id; // source node id
	int dst_id; // destiny node id

	bool path;

	struct Graph_Edge* next;
	struct Graph_Node* node; 

};


struct Alert{

	char src_hostname[50]; //hostname or ip address
	char dst_hostname[50]; //hostname or ip address

	double seconds_elapsed;

};

struct Alert_Path{ //Si

	char src_hostname[50]; //hostname or ip address
	char dst_hostname[50]; //hostname or ip address
		
	double seconds_elapsed;

	struct Alert_Path* next;
	struct Alert_Path* next_path;

};
	
typedef struct Graph_Node graph_node;
typedef struct Graph_Edge graph_edge;
typedef struct Alert_Path alert_path;
typedef struct Alert alert;


	
	
#endif	
