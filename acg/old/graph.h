#ifndef Graph_Structure
#define Graph_Structure

#include <stdbool.h>
#include <time.h>

struct Graph_Node{
	
	int id;
	char node_name[100];
	char hostname[50]; 

	bool vd_node; //result of exploit

	bool start_node; //where attacker starts

	int edge_number;
	int alerts_mapped;
	
	struct Graph_Node* next;
	struct Graph_Edge* edge;
	
	};

struct Graph_Edge{

	int src_id; // source node id
	int dst_id; // destiny node id

	bool path;

	struct Graph_Edge* next;
	struct Graph_Node* node; 

};

struct Mapped_SAG{

	struct Graph_Node* src_sag_node;
	struct Graph_Node* dst_sag_node;

	struct Mapped_SAG* position;
	struct Mapped_SAG* next;

};

struct Memory_IDs{ //mapped ids in each path inside acg

	int id;
	struct Memory_IDs* next;

};

struct Alert{

	char alert_src_ip[50]; //hostname or ip address
	char alert_dst_ip[50]; //hostname or ip address

	time_t alert_ts; // time of the alert

	struct Mapped_SAG* mapped_position;


};

struct ID_DATA{

	int id;
	bool full;

	struct ID_DATA* next;

};

struct Alert_Path{ //Si

	int src_id;
	int dst_id;
	
	time_t alert_ts; // time of the alert

	struct Alert_Path* next;
	struct Alert_Path* next_path;

	struct ID_DATA* connection_ids;

};
	
typedef struct Graph_Node graph_node;
typedef struct Graph_Edge graph_edge;
typedef struct Alert_Path alert_path;
typedef struct Mapped_SAG mapped_sag;
typedef struct Memory_IDs memory_ids;
typedef struct ID_DATA id_data;
typedef struct Alert alert;


	
	
#endif	
