#ifndef DATA
#define DATA

#include <stdbool.h>

struct Node{
	
	double pattern[19];
	double bitstring[19];
	double affinity;
	double fitness;
	double radius;	
	double cost;
	
	bool instance_class; // normal-> false / anomaly-> true
	
	int counter;
	int correct; // Number of anomalies correctly identified	
	int confirm;

	double seconds_elapsed;
		
	struct Node* next;
	struct Node* memory_cells;
	struct Node* best_detector;

	char src_ip[50];
	char dst_ip[50];
		
	};

typedef struct cells{

     double pattern[19];
     int confirm;

}cells;	
	
#endif	
