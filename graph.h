#ifndef Graph_Structure
#define Graph_Structure

#include <stdbool.h>
#include <time.h>

struct Payload{

	int id;
	int edge_count; // number of edges on each node

	char node_name[100];
	
	char src_hostname[50];
	char dst_hostname[50];

	char protocol[50];
	char port[50];
	char ip[50]; 

	bool vd_node; //result of exploit

	bool vc_node; // (Hosts, vul, alert) 

	bool leaf_node;

	bool start_node; //where attacker starts

	bool path;
	
	bool t_set;

	double prev_pr_T;
	double prev_pr_F;
	
	double pr_T;
	double pr_F;
	
	bool pr_set; //false -> node cpt not calculated / true -> it needs to calculate node cpt

};

struct Graph_Node{

	struct Payload data;
	
	struct Host_Data* hosts;	

	struct Graph_Node* next;
	
	struct Graph_Edge* edge;
	struct Graph_Edge* predecessor_edge;

	struct Graph_Node* parent_list;

	};

struct Host_Data{

	char src_hostname[50];
	char dst_hostname[50];

	struct Host_Data* next;

};

struct Graph_Edge{

	int src_id; // source node id
	int dst_id; // destiny node id

	bool path;

	struct Graph_Edge* next;
	struct Graph_Node* node; // Node where directed edge points to

};


struct Alert{

	char src_hostname[50]; //hostname or ip address
	char dst_hostname[50]; //hostname or ip address

	double seconds_elapsed;

};

struct alert_list{

	struct Alert a;
	struct alert_list* next;	

};

struct Alert_Path{ //Si

	char src_hostname[50]; //hostname or ip address
	char dst_hostname[50]; //hostname or ip address
		
	double seconds_elapsed;

	struct Alert_Path* next;
	struct Alert_Path* next_path;

};

struct CM_Data{

	int id;
	char* name;

	bool condition;

	double cost;
	double intrusiveness;
	double effectiveness;	

};

struct CM_Metric{

	int node_id;
	int cm_id;
	double value;

	struct CM_Metric* next;

};
	
typedef struct Graph_Node graph_node;
typedef struct Graph_Edge graph_edge;
typedef struct Alert_Path alert_path;
typedef struct Alert alert;
typedef struct alert_list alert_list;
typedef struct CM_Data cm_data;
typedef struct CM_Metric cm_metric;
typedef struct Host_Data host_data;
		
#endif	
