//#include "FreeRTOS.h"
//#include "task.h"
//#include "queue.h"
//#include "supporting_functions.h"
//#include <float.h>
//#include <limits.h>
//#include <math.h>
//#include <semphr.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <stdbool.h>
//#include <time.h>
//
//#define N 11						// number of malls
//#define INF FLT_MAX					// infinity to represent no direct path
//#define MAX_QUERIES 20000			// number of user quiries
//#define MAX_MOTORISTS 5000			// number of motorists
//#define MOTORIST_BATCH_SIZE 1000	// number of motorists in one batch
//#define MIN_SPEED 8					// minimum speed in km/h
//#define MAX_SPEED 150				// maximum speed in km/h
//
//// Queue management
//QueueHandle_t queryQueue;
//QueueHandle_t mtSpeedQueue;
//
//// Resource management
//SemaphoreHandle_t xMutex;
//
//// Timer management
//volatile TickType_t xStartTime;
//TickType_t queryStartTimes[MAX_QUERIES];
//TickType_t queryEndTimes[MAX_QUERIES];
//
//// Define the source and destination
//#define SRC_NODE 3
//#define DST_NODE 5
//
//// User selection
//int sel = 0;
//
//// User query generation struct
//typedef struct {
//	int srcNode;
//	int dstNode;
//} query_t;
//
//// Motorists path and speed generation struct
//typedef struct {
//	int srcNode;
//	int dstNode;
//	float mtSpeed; // motorist speed
//} motorist_t;
//
//typedef struct {
//	motorist_t motorists[MOTORIST_BATCH_SIZE];
//} motorist_batch_t;
//
//// Traffic matrix's struct data
//typedef struct {
//	float trfCondition;	// traffic condition {1.0 <= x <= 2.0}
//	//float travelCost;	// travel cost (distance * traffic condition)
//} trafficRoute_t;
//
//// Average speed matrix's struct data
//typedef struct {
//	float pathCount;	// number of motorist on a path
//	float accSpeed;		// accumulated speed of motorist on a path
//	float avgSpeed;		// average speed (accumulated speed / number of motorist)
//} speedRoute_t;
//
//// Average time matrix's struct data
//typedef struct {
//	float avgTrvTime;	// average traveling time on a path (distance / average Speed)
//} timeRoute_t;
//
//// A* node structure
//typedef struct {
//	int vertex;
//	float gCost; // Distance from start node
//	float hCost; // Heuristic cost to goal
//	float fCost; // Total cost (gCost + hCost)
//} AStarNode;
//
//// Heuristic function using the euclidean distance
//float heuristic(int src, int goal) {
//	float x1, x2, y1, y2;
//	x1 = src / 4;
//	y1 = src % 4;
//	x2 = goal / 4;
//	y2 = goal % 4;
//	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
//}
//
//// List node for priority queue
//typedef struct ListNode {
//	AStarNode* data;
//	struct ListNode* next;
//} ListNode;
//
//// Function declarations
//ListNode* insertSorted(ListNode* head, AStarNode* newNode);
//float heuristic(int src, int goal);
//bool isEmptyPriorityQueue(ListNode* head);
//AStarNode* extractMinPriorityQueue(ListNode** head);
//
//// Heap node structure
//typedef struct {
//	int vertex;			// vertex number
//	float distance;		// distance from source
//} HeapNode;
//
//// Min heap structure
//typedef struct {
//	int size;			// current number of heap nodes
//	int capacity;		// total capacity of heap
//	int* pos;			// keeps track of positions of heap nodes in pos[]
//	HeapNode** array;	// array of pointers to heap nodes
//} MinHeap;
//
//// A* Algorithm Function
//void aStar(int start, int goal, float dist[], int prev[], float graph[N][N]) {
//	// A* specific initialization
//	for (int i = 0; i < N; i++) {
//		dist[i] = INF;
//		prev[i] = -1;
//	}
//
//	// Use a list to store AStarNodes as a priority queue
//	ListNode* priorityQueue = NULL;
//
//	// Create start node and add to the priority queue
//	AStarNode* startNode = (AStarNode*)malloc(sizeof(AStarNode));
//	startNode->vertex = start;
//	startNode->gCost = 0;
//	startNode->hCost = heuristic(start, goal);
//	startNode->fCost = startNode->gCost + startNode->hCost;
//
//	priorityQueue = insertSorted(priorityQueue, startNode);
//
//	// Main A* loop
//	while (!isEmpty(priorityQueue)) {
//		// Extract node with the smallest fCost
//		AStarNode* currentNode = extractMinPriorityQueue(&priorityQueue);
//
//		// If the current node is the goal, break
//		if (currentNode->vertex == goal) {
//			free(currentNode);
//			break;
//		}
//
//		// Iterate over neighbors of currentNode
//		for (int i = 0; i < N; i++) {
//			// Check if the neighbor is valid (path exists)
//			if (graph[currentNode->vertex][i] != INF) {
//				float newGCost = currentNode->gCost + graph[currentNode->vertex][i];
//				float newHCost = heuristic(i, goal);
//				float newFCost = newGCost + newHCost;
//
//				// Check if this path is better than any previous one
//				if (newFCost < dist[i]) {
//					dist[i] = newFCost;
//					prev[i] = currentNode->vertex;
//
//					// Create a new node and add/update it in the priority queue
//					AStarNode* neighborNode = (AStarNode*)malloc(sizeof(AStarNode));
//					neighborNode->vertex = i;
//					neighborNode->gCost = newGCost;
//					neighborNode->hCost = newHCost;
//					neighborNode->fCost = newFCost;
//
//					priorityQueue = insertSorted(priorityQueue, neighborNode);
//				}
//			}
//		}
//
//		free(currentNode);
//	}
//
//	// Cleanup: Free any remaining nodes in the priority queue
//	while (!isEmptyPriorityQueue(priorityQueue)) {
//		ListNode* temp = priorityQueue;
//		priorityQueue = priorityQueue->next;
//		free(temp->data);
//		free(temp);
//	}
//}
//
//// Priority Queue Management Functions
//bool isEmptyPriorityQueue(ListNode* head) {
//	return head == NULL;
//}
//
//ListNode* insertSorted(ListNode* head, AStarNode* newNode) {
//	ListNode* newListNode = (ListNode*)malloc(sizeof(ListNode));
//	newListNode->data = newNode;
//	newListNode->next = NULL;
//
//	if (head == NULL || head->data->fCost >= newNode->fCost) {
//		newListNode->next = head;
//		head = newListNode;
//	}
//	else {
//		ListNode* current = head;
//		while (current->next != NULL && current->next->data->fCost < newNode->fCost) {
//			current = current->next;
//		}
//		newListNode->next = current->next;
//		current->next = newListNode;
//	}
//
//	return head;
//}
//
//AStarNode* extractMinPriorityQueue(ListNode** head) {
//	if (*head == NULL) return NULL;
//
//	ListNode* temp = *head;
//	*head = (*head)->next;
//
//	AStarNode* minNode = temp->data;
//	free(temp);
//
//	return minNode;
//}
//
//// Function to create a new min heap node
//HeapNode* newHeapNode(int v, float dist) {
//	HeapNode* heapNode = (HeapNode*)malloc(sizeof(HeapNode)); // allocate memory for heap node
//	heapNode->vertex = v;
//	heapNode->distance = dist;
//	return heapNode;
//}
//
//// Function to create a min heap
//MinHeap* createMinHeap(int capacity) {
//	MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
//	minHeap->pos = (int*)malloc(capacity * sizeof(int));
//	minHeap->size = 0; // initialize size as 0 (heap is empty)
//	minHeap->capacity = capacity;
//	minHeap->array = (HeapNode**)malloc(capacity * sizeof(HeapNode*)); // array is the actual heap
//	return minHeap;
//}
//
//// Function to swap two nodes of the min heap
//void swapMinHeapNode(HeapNode** a, HeapNode** b) {
//	HeapNode* t = *a;
//	*a = *b;
//	*b = t;
//}
//
//// A standard function to heapify at given idx
//void minHeapify(MinHeap* minHeap, int idx) {
//	int smallest, left, right;
//	smallest = idx;
//	left = 2 * idx + 1;
//	right = 2 * idx + 2;
//
//	if (left < minHeap->size && minHeap->array[left]->distance < minHeap->array[smallest]->distance)
//		smallest = left;
//
//	if (right < minHeap->size && minHeap->array[right]->distance < minHeap->array[smallest]->distance)
//		smallest = right;
//
//	if (smallest != idx) {
//		// The nodes to be swapped in min heap
//		HeapNode* smallestNode = minHeap->array[smallest];
//		HeapNode* idxNode = minHeap->array[idx];
//
//		// Swap positions
//		minHeap->pos[smallestNode->vertex] = idx;
//		minHeap->pos[idxNode->vertex] = smallest;
//
//		// Swap nodes
//		swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
//
//		minHeapify(minHeap, smallest);
//	}
//}
//
//// Function to check if the given minHeap is empty or not
//int isEmpty(MinHeap* minHeap) {
//	return minHeap->size == 0;
//}
//
//// Standard function to extract minimum node from heap
//HeapNode* extractMin(MinHeap* minHeap) {
//	if (isEmpty(minHeap))
//		return NULL;
//
//	// Store the root node
//	HeapNode* root = minHeap->array[0];
//
//	// Replace root node with last node
//	HeapNode* lastNode = minHeap->array[minHeap->size - 1];
//	minHeap->array[0] = lastNode;
//
//	// Update position of last node
//	minHeap->pos[root->vertex] = minHeap->size - 1;
//	minHeap->pos[lastNode->vertex] = 0;
//
//	// Reduce heap size and heapify root
//	--minHeap->size;
//	minHeapify(minHeap, 0);
//
//	return root;
//}
//
//// Function to decrease distance value of a given vertex v
//void decreaseKey(MinHeap* minHeap, int v, float dist) {
//	// Get the index of v in heap array
//	int i = minHeap->pos[v];
//
//	// Get the node and update its distance value
//	minHeap->array[i]->distance = dist;
//
//	// Travel up while the complete tree is not heapified
//	while (i && minHeap->array[i]->distance < minHeap->array[(i - 1) / 2]->distance) {
//		// Swap this node with its parent
//		minHeap->pos[minHeap->array[i]->vertex] = (i - 1) / 2;
//		minHeap->pos[minHeap->array[(i - 1) / 2]->vertex] = i;
//		swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);
//
//		// move to parent index
//		i = (i - 1) / 2;
//	}
//}
//
//// Function to check if a given vertex 'v' is in min heap or not
//bool isInMinHeap(MinHeap* minHeap, int v) {
//	if (minHeap->pos[v] < minHeap->size)
//		return true;
//	return false;
//}
//
//// Mall names
//const char* mallNames[N] = {
//	"AMTM", "APBJ", "AKSM", "BSC", "BTS", "BMSC", "BBP", "DCM", "EP", "F88", "GDSM"
//};
//
//// Distance matrix (in km, INF = no direct path) - Static
//static float distanceMatrix[N][N] = {
//	{INF, 14.5, 7.6, 13, 4, 15.2, INF, INF, 14.8, 5.4, 17.7},       // AMTM
//	{14.6, INF, 17.4, 13.6, 14.4, 26.1, INF, INF, 5.3, INF, 14.5},  // APBJ
//	{7.3, 22.1, INF, 9.6, INF, 10.9, INF, INF, 15.2, INF, 16.4},    // AKSM
//	{11.2, 13.1, INF, INF, INF, 9.9, INF, 1.7, 16.4, INF, 5.5},     // BSC
//	{3.4, INF, INF, 9.1, INF, 13, INF, INF, 12.5, INF, 15.9},       // BTS
//	{17.4, 22.8, INF, 11.1, INF, INF, INF, INF, 22.7, INF, 12.2},   // BMSC
//	{INF, INF, INF, 8, INF, 10.5, INF, INF, 12.6, INF, 12.7},       // BBP
//	{11.5, 16.9, INF, INF, 8.5, 9.4, 9.3, INF, 18.4, INF, 6.2},     // DCM
//	{11.5, 4, 14.9, 14.8, 11.9, 20.7, INF, INF, INF, INF, 16.7},    // EP
//	{INF, INF, INF, INF, 0.85, INF, INF, INF, 13.3, INF, INF},      // F88
//	{16.2, 17.3, 14, 6.6, INF, 13.1, INF, INF, 18.5, INF, INF}      // GDSM
//};
//
//// Traffic matrix - Dynamic 
//trafficRoute_t trafficMatrix[N][N];
//
//// Average speed matrix - Dynamic
//speedRoute_t avgSpeedMatrix[N][N];
//
//// Average time matrix - Dynamic
//timeRoute_t avgTimeMatrix[N][N];
//
//// Initialize random average speed matrix (avgSpeedMatrix)
//void initAverageSpeedMatrix() {
//	srand((unsigned)time(NULL)); // Seed for random number generation
//	for (int i = 0; i < N; i++) {
//		for (int j = 0; j < N; j++) {
//			if (distanceMatrix[i][j] != INF) {
//				// Generate a random speed between 8 and 150 km/h
//				avgSpeedMatrix[i][j].avgSpeed = MIN_SPEED + (rand() % (MAX_SPEED - MIN_SPEED + 1));
//				// Initialize pathCount and accSpeed
//				avgSpeedMatrix[i][j].pathCount = 1;
//				avgSpeedMatrix[i][j].accSpeed = avgSpeedMatrix[i][j].avgSpeed;
//			}
//			else {
//				// Set to INF if no direct path
//				avgSpeedMatrix[i][j].avgSpeed = INF;
//				avgSpeedMatrix[i][j].pathCount = INF;
//				avgSpeedMatrix[i][j].accSpeed = INF;
//			}
//		}
//	}
//}
//
//// Initialize traffic condition using piecewise function (trafficMatrix)
//void initTrafficMatrix() {
//	for (int i = 0; i < N; i++) {
//		for (int j = 0; j < N; j++) {
//			if (distanceMatrix[i][j] != INF) {
//				float speed = avgSpeedMatrix[i][j].avgSpeed;
//
//				if (speed >= 131.6 && speed <= 150.0) trafficMatrix[i][j].trfCondition = 1.0;
//				else if (speed >= 118.1 && speed < 131.6) trafficMatrix[i][j].trfCondition = 1.1;
//				else if (speed >= 104.6 && speed < 118.1) trafficMatrix[i][j].trfCondition = 1.2;
//				else if (speed >= 91.1 && speed < 104.6) trafficMatrix[i][j].trfCondition = 1.3;
//				else if (speed >= 77.6 && speed < 91.1) trafficMatrix[i][j].trfCondition = 1.4;
//				else if (speed >= 64.1 && speed < 77.6) trafficMatrix[i][j].trfCondition = 1.5;
//				else if (speed >= 50.6 && speed < 64.1) trafficMatrix[i][j].trfCondition = 1.6;
//				else if (speed >= 37.1 && speed < 50.6) trafficMatrix[i][j].trfCondition = 1.7;
//				else if (speed >= 23.6 && speed < 37.1) trafficMatrix[i][j].trfCondition = 1.8;
//				else if (speed >= 10.1 && speed < 23.6) trafficMatrix[i][j].trfCondition = 1.9;
//				else if (speed >= 8.0 && speed < 10.1) trafficMatrix[i][j].trfCondition = 2.0;
//				else trafficMatrix[i][j].trfCondition = INF;  // In case the speed is outside the defined ranges
//			}
//			else {
//				trafficMatrix[i][j].trfCondition = INF;
//			}
//		}
//	}
//}
//
//// Initialize average traveling time matrix (avgTimeMatrix)
//void initAverageTrvTimeMatrix() {
//	for (int i = 0; i < N; i++) {
//		for (int j = 0; j < N; j++) {
//			if (distanceMatrix[i][j] != INF) {
//				// Average traveling time (in minutes) = (distance / average speed) x 60 minutes
//				avgTimeMatrix[i][j].avgTrvTime = (distanceMatrix[i][j] / avgSpeedMatrix[i][j].avgSpeed) * 60;
//			}
//			else {
//				// Set to INF if no direct path
//				avgTimeMatrix[i][j].avgTrvTime = INF;
//			}
//		}
//	}
//}
//
//// TO TEST TO PRINT THE MATRIXXX ---------------------------------------- (DELETE LATER)
//// Print avg speed
//void printAvgSpeed() {
//	printf("Average Speed Matrix:\n");
//	for (int i = 0; i < N; i++) {
//		for (int j = 0; j < N; j++) {
//			if (avgTimeMatrix[i][j].avgTrvTime == INF)
//				printf("INF\t\t");
//			else
//				printf("%.2f\t\t", avgSpeedMatrix[i][j].avgSpeed);
//		}
//		printf("\n");
//	}
//	printf("\n");
//}
//
//// TO TEST TO PRINT THE MATRIXXX
//// Print avg time
//void printAvgTrvTime() {
//	printf("Average Speed Matrix:\n");
//	for (int i = 0; i < N; i++) {
//		for (int j = 0; j < N; j++) {
//			if (avgTimeMatrix[i][j].avgTrvTime == INF)
//				printf("INF\t\t");
//			else
//				printf("%.2f\t\t", avgTimeMatrix[i][j].avgTrvTime);
//		}
//		printf("\n");
//	}
//	printf("\n");
//}
//
//// TO TEST TO PRINT THE MATRIXXX
//// Print traffic condition
//void printTrafficCond() {
//	printf("Average Speed Matrix:\n");
//	for (int i = 0; i < N; i++) {
//		for (int j = 0; j < N; j++) {
//			if (trafficMatrix[i][j].trfCondition == INF)
//				printf("INF\t\t");
//			else
//				printf("%.2f\t\t", trafficMatrix[i][j].trfCondition);
//		}
//		printf("\n");
//	}
//	printf("\n");
//}
//
//// Dijkstra's Algorithm (Standard)
//void dijkstra(int src, float dist[], int prev[], timeRoute_t graph[N][N]) {
//	bool visited[N];
//
//	for (int i = 0; i < N; i++) {
//		dist[i] = INF;
//		prev[i] = -1;
//		visited[i] = false;
//	}
//
//	dist[src] = 0;
//
//	// Find shortest path for all vertices
//	for (int count = 0; count < N - 1; count++) {
//		int u = -1;
//		float minDist = INF;
//
//		for (int v = 0; v < N; v++) {
//			if (!visited[v] && dist[v] <= minDist) {
//				minDist = dist[v];
//				u = v;
//			}
//		}
//
//		// Mark the picked vertex as visited
//		visited[u] = true;
//
//		// Update dist value of the adjacent vertices of the picked vertex
//		for (int v = 0; v < N; v++) {
//			if (!visited[v] && graph[u][v].avgTrvTime != INF && dist[u] + graph[u][v].avgTrvTime < dist[v]) {
//				prev[v] = u;
//				dist[v] = dist[u] + graph[u][v].avgTrvTime;
//			}
//		}
//	}
//}
//
//// Dijkstra's Algorithm (With Priority Queue)
//void dijkstraPQ(int src, float dist[], int prev[], timeRoute_t graph[N][N]) {
//	MinHeap* minHeap = createMinHeap(N);
//
//	for (int v = 0; v < N; v++) {
//		dist[v] = INF;
//		prev[v] = -1;
//		minHeap->array[v] = newHeapNode(v, dist[v]);
//		minHeap->pos[v] = v;
//	}
//
//	// Make distance value of src vertex as 0 so it is extracted first
//	dist[src] = 0;
//	decreaseKey(minHeap, src, dist[src]);
//
//	minHeap->size = N;
//
//	while (!isEmpty(minHeap)) {
//		HeapNode* minHeapNode = extractMin(minHeap);
//		int u = minHeapNode->vertex;
//
//		for (int v = 0; v < N; v++) {
//			if (isInMinHeap(minHeap, v) && graph[u][v].avgTrvTime != INF && dist[u] != INF && dist[u] + graph[u][v].avgTrvTime < dist[v]) {
//				dist[v] = dist[u] + graph[u][v].avgTrvTime;
//				prev[v] = u;
//				decreaseKey(minHeap, v, dist[v]);
//			}
//		}
//	}
//}
//
//// Print shortest path
//void printShortestPath(int prev[], int src, int vertex, const char* mallNames[]) {
//	if (vertex == src) {
//		printf("%s", mallNames[vertex]);
//		return;
//	}
//
//	if (prev[vertex] == -1) {
//		return INF;
//	}
//
//	printShortestPath(prev, src, prev[vertex], mallNames);
//	printf(" -> %s", mallNames[vertex]);
//}
//
//// Calculate total distance to print the initial distance value to user
//float totalDistCalculation(int prev[], int src, int vertex) {
//	if (vertex == src) {
//		return 0;
//	}
//
//	if (prev[vertex] == -1) {
//		return INF;
//	}
//
//	return distanceMatrix[prev[vertex]][vertex] + totalDistCalculation(prev, src, prev[vertex]);
//}
//
//// User Query Generation Task (Sender Task)
//static void vUserQueryTask(void* pvParameters) {
//	srand((unsigned)time(NULL));
//	query_t lValueToSend;
//	BaseType_t xStatus;
//
//	// Random generate source and destination query and user speed
//	for (unsigned int i = 0; i < MAX_QUERIES; i++) {
//		
//		// Generate from fixed source to fixed destination
//		if (sel == 2 || sel == 4 || sel == 6) {
//			lValueToSend.srcNode = SRC_NODE;
//			lValueToSend.dstNode = DST_NODE;
//		}
//		else {
//			lValueToSend.srcNode = rand() % N;
//			do {
//				lValueToSend.dstNode = rand() % N;
//			} while (lValueToSend.srcNode == lValueToSend.dstNode);
//		}
//
//		queryStartTimes[i] = xTaskGetTickCount();
//
//		// Send data to the queryQueue
//		/* 100 represents the maximum time the task should block waiting for space
//		to become available on the queue if the queue is already full */
//		xStatus = xQueueSendToBack(queryQueue, &lValueToSend, portMAX_DELAY);
//
//		if (xStatus != pdPASS) {
//			vPrintString("Could not send to the queue. \r\n");
//		}
//	}
//	vTaskDelete(NULL);
//}
//
//// Path Calculation Task (Receiver Task)
//static void vPathCalculationTask(void* pvParameters) {
//	query_t	lReceivedValue;
//	BaseType_t xStatus;
//	float dist[N];
//	int prev[N]; //precedessor array which stores the precedessor vertex
//	unsigned int i = 0;
//
//	// Variables to hold timing information for each batch
//	TickType_t batchStartTime = xTaskGetTickCount();
//	TickType_t batchTotalTime = 0;
//	float batchMinTime = FLT_MAX;
//	float batchMaxTime = 0.0;
//
//	while (true) {
//		xStatus = xQueueReceive(queryQueue, &lReceivedValue, portMAX_DELAY);
//
//		if (xStatus == pdPASS) {
//			// Take the mutex before perform the operations
//			if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
//				if (sel == 1) {
//					dijkstra(lReceivedValue.srcNode, dist, prev, avgTimeMatrix);
//				}
//				else if (sel == 2) {
//					dijkstra(lReceivedValue.srcNode, dist, prev, avgTimeMatrix);
//				}
//				else if (sel == 3) {
//					dijkstraPQ(lReceivedValue.srcNode, dist, prev, avgTimeMatrix);
//				}
//				else if (sel == 4) {
//					dijkstraPQ(lReceivedValue.srcNode, dist, prev, avgTimeMatrix);
//				}
//				else if (sel == 5) {
//					aStar(lReceivedValue.srcNode, lReceivedValue.dstNode, dist, prev, avgTimeMatrix);
//				}
//				else if (sel == 6) {
//					aStar(lReceivedValue.srcNode, lReceivedValue.dstNode, dist, prev, avgTimeMatrix);
//				}
//				else {
//					vTaskEndScheduler();
//				}
//
//				// Release the mutex 
//				xSemaphoreGive(xMutex);
//			}
//
//			// Update timing information
//			TickType_t queryEndTime = xTaskGetTickCount();
//			TickType_t queryElapsedTime = queryEndTime - queryStartTimes[i];
//			batchTotalTime += queryElapsedTime;
//
//			// Convert ticks to milliseconds for min and max times
//			float queryDurationInMs = (float)queryElapsedTime * portTICK_PERIOD_MS;
//
//			// Update min and max times
//			if (queryDurationInMs < batchMinTime) {
//				batchMinTime = queryDurationInMs;
//			}
//			if (queryDurationInMs > batchMaxTime) {
//				batchMaxTime = queryDurationInMs;
//			}
//
//			i++;
//
//			if (sel == 2 || sel == 4 || sel == 6) {
//				//// ------------------------------------------------------- TO TEST TO PRINT THE MATRIXXX ----------------------------------------
//				//// Print generated query and user speed
//				//printf("User %d\t: %s -> %s\n",
//				//	i,
//				//	mallNames[lReceivedValue.srcNode],
//				//	mallNames[lReceivedValue.dstNode]);
//
//				//// Print shortest path
//				//printf("Shortest Path\t: ");
//				//printShortestPath(prev, lReceivedValue.srcNode, lReceivedValue.dstNode, mallNames);
//
//				//// Print total distance
//				//float totalDistance = totalDistCalculation(prev, lReceivedValue.srcNode, lReceivedValue.dstNode);
//				//printf("\nDistance\t: %.2f km\n", totalDistance);
//
//				// Calculate and print ETA
//				float totalETA = 0.0;
//				int currentVertex = lReceivedValue.dstNode;
//				while (currentVertex != lReceivedValue.srcNode && prev[currentVertex] != -1) {
//					totalETA += avgTimeMatrix[prev[currentVertex]][currentVertex].avgTrvTime;
//					currentVertex = prev[currentVertex];
//				}
//				//printf("ETA\t\t: %.0f minute[s]\n\n", (fmod(totalETA, 1) < 0.4) ? floor(totalETA) : ceil(totalETA));
//
//				if (i % 2000 == 0) {
//					// Print the checking progress in percentage
//					printf("Checking Progress: %.2f%%\n", (float)i / MAX_QUERIES * 100);
//				}
//
//				if (i == MAX_QUERIES) {
//					vPrintString("\n");
//					vPrintString("               Accuracy Checking               \n");
//					vPrintString("-----------------------------------------------\n");
//
//					// Print the source and destination
//					printf("Source\t\t\t: %s\n", mallNames[lReceivedValue.srcNode]);
//					printf("Destination\t\t: %s\n", mallNames[lReceivedValue.dstNode]);
//
//					vPrintString("-----------------------------------------------\n");
//
//					// Print the direct path for the selected source and destination
//					printf("Direct Path\t\t: ");
//					if (distanceMatrix[lReceivedValue.srcNode][lReceivedValue.dstNode] != INF) {
//						printf("%s -> %s\n", mallNames[lReceivedValue.srcNode], mallNames[lReceivedValue.dstNode]);
//
//						// Print the total distance for the direct path
//						printf("Total Distance\t\t: %.2f km\n", distanceMatrix[lReceivedValue.srcNode][lReceivedValue.dstNode]);
//
//						// Print the ETA for the direct path
//						printf("ETA\t\t\t: %.2f minute[s]\n", avgTimeMatrix[lReceivedValue.srcNode][lReceivedValue.dstNode].avgTrvTime);
//
//						vPrintString("-----------------------------------------------\n");
//					}
//					else {
//						printf("%s -> %s\n", mallNames[lReceivedValue.srcNode], mallNames[lReceivedValue.dstNode]);
//
//						// Print the total distance for the direct path
//						printf("Total Distance\t\t: INF\n");
//
//						// Print the ETA for the direct path
//						printf("ETA\t\t\t: INF\n");
//
//						vPrintString("-----------------------------------------------\n");
//					}
//
//					//// Print the total distance for the direct path
//					//printf("Total Distance\t\t: %.2f km\n", distanceMatrix[lReceivedValue.srcNode][lReceivedValue.dstNode]);
//
//					//// Print the average speed for the direct path
//					//printf("Average Speed\t\t: %.2f km/h\n", avgSpeedMatrix[lReceivedValue.srcNode][lReceivedValue.dstNode].avgSpeed);
//
//					//// Print the traffic condition for the direct path
//					//printf("Traffic Condition\t: %.2f\n", trafficMatrix[lReceivedValue.srcNode][lReceivedValue.dstNode].trfCondition);
//					//
//					//// Print the ETA for the direct path
//					//printf("ETA\t\t\t: %.2f minute[s]\n", avgTimeMatrix[lReceivedValue.srcNode][lReceivedValue.dstNode].avgTrvTime);
//
//					//vPrintString("-----------------------------------------------\n");
//
//					// Print the recommended path for the selected source and destination (may have intermediate nodes)
//					printf("Recommended Path\t: ");
//					printShortestPath(prev, lReceivedValue.srcNode, lReceivedValue.dstNode, mallNames);
//
//					// Print the total distance for the recommended path
//					printf("\nTotal Distance\t\t: %.2f km\n", totalDistCalculation(prev, lReceivedValue.srcNode, lReceivedValue.dstNode));
//
//					// Print the ETA for the recommended path
//					printf("ETA\t\t\t: %.2f minute[s]\n", totalETA);
//
//					vPrintString("-----------------------------------------------\n");
//
//					//// Print the total distance for the recommended path
//					//printf("\nTotal Distance\t\t: %.2f km\n", totalDistCalculation(prev, lReceivedValue.srcNode, lReceivedValue.dstNode));
//
//					//// Print the average speed for the recommended path
//					//printf("Average Speed\t\t: %.2f km/h\n", avgSpeedMatrix[lReceivedValue.srcNode][lReceivedValue.dstNode].avgSpeed);
//
//					//// Print the traffic condition for the recommended path
//					//printf("Traffic Condition\t: %.2f\n", trafficMatrix[lReceivedValue.srcNode][lReceivedValue.dstNode].trfCondition);
//
//					//// Print the ETA for the recommended path
//					//printf("ETA\t\t\t: %.2f minute[s]\n", avgTimeMatrix[lReceivedValue.srcNode][lReceivedValue.dstNode].avgTrvTime);
//
//					//vPrintString("-----------------------------------------------\n");
//					vTaskEndScheduler();
//				}
//			}
//			else if (sel == 1 || sel == 3 || sel == 5) {
//				// Check if a new batch of 1000 queries is completed or the end is reached
//				if (i % 2000 == 0 || i == MAX_QUERIES) {
//					// Calculate cumulative average time
//					float cumulativeAverageTime = (batchTotalTime / (float)i) * portTICK_PERIOD_MS;
//
//					//// ------------------------------------------------------- TO TEST TO PRINT THE MATRIXXX ----------------------------------------
//					//// Print generated query and user speed
//					//printf("User %d\t: %s -> %s\n",
//					//	i,
//					//	mallNames[lReceivedValue.srcNode],
//					//	mallNames[lReceivedValue.dstNode]);
//
//					//// Print shortest path
//					//printf("Shortest Path\t: ");
//					//printShortestPath(prev, lReceivedValue.srcNode, lReceivedValue.dstNode, mallNames);
//
//					//// Print total distance
//					//float totalDistance = totalDistCalculation(prev, lReceivedValue.srcNode, lReceivedValue.dstNode);
//					//printf("\nDistance\t: %.2f km\n", totalDistance);
//
//					// Calculate and print ETA
//
//					float totalETA = 0.0;
//					int currentVertex = lReceivedValue.dstNode;
//					while (currentVertex != lReceivedValue.srcNode && prev[currentVertex] != -1) {
//						totalETA += avgTimeMatrix[prev[currentVertex]][currentVertex].avgTrvTime;
//						currentVertex = prev[currentVertex];
//					}
//					//printf("ETA\t\t: %.0f minute[s]\n\n", (fmod(totalETA, 1) < 0.4) ? floor(totalETA) : ceil(totalETA));
//
//					if (i == 2000) {
//						// Print all the elapsed times
//						vPrintString("                                    Performance Evaluation                                    \n");
//						vPrintString("----------------------------------------------------------------------------------------------\n");
//						vPrintString("| User Queries | Total Elapsed Time | Min Elapsed Time | Max Elapsed Time | Avg Elapsed Time |");
//						vPrintString("\n----------------------------------------------------------------------------------------------\n");
//					}
//
//					printf("| %d\t       | %d ms\t\t    | %.2f ms\t       | %.2f ms\t  | %.4f ms\t     |\n",
//						i, batchTotalTime, batchMinTime, batchMaxTime, cumulativeAverageTime);
//
//					if (i == MAX_QUERIES) {
//						vPrintString("----------------------------------------------------------------------------------------------\n\n");
//						printf("User Queries\t\t: %d\n", i);
//						printf("Total Elapsed Time\t: %d ms\n", batchTotalTime);
//						printf("Min Elapsed Time\t: %.2f ms\n", batchMinTime);
//						printf("Max Elapsed Time\t: %.2f ms\n", batchMaxTime);
//						printf("Avg Elapsed Time\t: %.4f ms\n", cumulativeAverageTime);
//						vPrintStringAndNumber("\nUser Selection:", sel);
//						vTaskEndScheduler();
//					}
//				}
//			}
//			else {
//				vTaskEndScheduler();
//			}
//		}
//		else {
//			vPrintString("Could not receive from the queue. \r\n");
//		}
//	}
//}
//
//// Motorist Path & Speed Generation Task (Sender Task)
//static void vMotoristDataTask(void* pvParameters) {
//	srand((unsigned)time(NULL));
//	motorist_batch_t motoristBatch;
//	unsigned int batchIndex = 0;
//	BaseType_t xStatus;
//
//	// Random generate path and speed
//	while (true) {
//		motorist_t lValueToSend;
//
//		// Generate random single path
//		lValueToSend.srcNode = rand() % N;
//		do {
//			lValueToSend.dstNode = rand() % N;
//		} while (lValueToSend.srcNode == lValueToSend.dstNode || distanceMatrix[lValueToSend.srcNode][lValueToSend.dstNode] == INF);
//
//		// Retrieve the traffic condition for the direct path
//		float trfCondition = trafficMatrix[lValueToSend.srcNode][lValueToSend.dstNode].trfCondition;
//
//		// Generate speed randomly based on traffic condition with the piecewise function
//		float minSpeed = MIN_SPEED;
//		float maxSpeed = MAX_SPEED;
//		if (trfCondition == 1.0) { minSpeed = 131.6; maxSpeed = 150.0; }
//		else if (trfCondition == 1.1) { minSpeed = 118.1; maxSpeed = 136.5; }
//		else if (trfCondition == 1.2) { minSpeed = 104.6; maxSpeed = 118.0; }
//		else if (trfCondition == 1.3) { minSpeed = 91.1; maxSpeed = 104.5; }
//		else if (trfCondition == 1.4) { minSpeed = 77.6; maxSpeed = 91.0; }
//		else if (trfCondition == 1.5) { minSpeed = 64.1; maxSpeed = 77.5; }
//		else if (trfCondition == 1.6) { minSpeed = 50.6; maxSpeed = 64.0; }
//		else if (trfCondition == 1.7) { minSpeed = 37.1; maxSpeed = 50.5; }
//		else if (trfCondition == 1.8) { minSpeed = 23.6; maxSpeed = 37.0; }
//		else if (trfCondition == 1.9) { minSpeed = 10.1; maxSpeed = 23.5; }
//		else if (trfCondition == 2.0) { minSpeed = 8.0; maxSpeed = 10.0; }
//
//		// Generate a random speed
//		lValueToSend.mtSpeed = minSpeed + ((float)rand() / RAND_MAX) * (maxSpeed - minSpeed);
//		//printf("User %d\t\t: %s -> %s\n",
//		//	i + 1,
//		//	mallNames[lValueToSend.srcNode],
//		//	mallNames[lValueToSend.dstNode]);
//		//printf("Motorist speed\t\t: %.2f\n\n", lValueToSend.mtSpeed);
//
//		// Add the generated motorist data to the batch
//		motoristBatch.motorists[batchIndex++] = lValueToSend;
//
//		// Check if the batch is full
//		if (batchIndex >= MOTORIST_BATCH_SIZE) {
//			// Send the full batch to the queue
//			xStatus = xQueueSendToBack(mtSpeedQueue, &motoristBatch, portMAX_DELAY);
//
//			if (xStatus != pdPASS) {
//				vPrintString("Could not send to the queue. \r\n");
//			}
//
//			// Reset the batch index for the next batch
//			batchIndex = 0;
//		}
//	}
//}
//
//// THESE FUNCTIONS ARE MAINLY FOR THE TASK (vUpdateMatricesTask)
//// Update average matrix
//void uptAvgSpeedMatrix(int srcNode, int dstNode, float mtSpeed) {
//
//	if (distanceMatrix[srcNode][dstNode] != INF) {
//		avgSpeedMatrix[srcNode][dstNode].pathCount += 1;
//		avgSpeedMatrix[srcNode][dstNode].accSpeed += mtSpeed;
//		avgSpeedMatrix[srcNode][dstNode].avgSpeed = avgSpeedMatrix[srcNode][dstNode].accSpeed / avgSpeedMatrix[srcNode][dstNode].pathCount;
//
//		// Increase traffic condition by 0.1 for every 10 motorists
//		if ((int)avgSpeedMatrix[srcNode][dstNode].pathCount % 10 == 0) {
//			trafficMatrix[srcNode][dstNode].trfCondition += 0.1;
//
//			// Cap the traffic condition to a maximum (e.g., 2.0)
//			if (trafficMatrix[srcNode][dstNode].trfCondition > 2.0) {
//				trafficMatrix[srcNode][dstNode].trfCondition = 2.0;
//			}
//		}
//	}
//}
//
//// Update average traveling time
//void uptAvgTrvTimeMatrix(int srcNode, int dstNode) {
//	if (distanceMatrix[srcNode][dstNode]) {
//		// Average traveling time (in minutes) = (distance / average speed) * 60
//		avgTimeMatrix[srcNode][dstNode].avgTrvTime = (distanceMatrix[srcNode][dstNode] / avgSpeedMatrix[srcNode][dstNode].avgSpeed) * 60;
//	}
//}
//
//// Update Matrices Task (Receiver Task)
//static void vUpdateMatricesTask(void* pvParameters) {
//	motorist_batch_t lReceivedValue;
//	BaseType_t xStatus;
//
//	// Declare a variable to hold the last wake time
//	TickType_t xLastWakeTime;
//
//	// Initialize the xLastWakeTime variable with the current time
//	xLastWakeTime = xTaskGetTickCount();
//
//	while (true) {
//		// Wait for the next cycle
//		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(500));
//
//		xStatus = xQueueReceive(mtSpeedQueue, &lReceivedValue, portMAX_DELAY);
//
//		if (xStatus == pdPASS) {
//			// Take the mutex before perform the operations
//			if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
//				// Process each motorist in the received batch
//				for (unsigned int i = 0; i < MOTORIST_BATCH_SIZE; i++) {
//
//					motorist_t motorist = lReceivedValue.motorists[i];
//					int src = motorist.srcNode;
//					int dst = motorist.dstNode;
//					float mtSpeed = motorist.mtSpeed;
//
//					// Update the average speed and traveling time matrices
//					uptAvgSpeedMatrix(src, dst, mtSpeed);
//					uptAvgTrvTimeMatrix(src, dst);
//
//					//// Log updates at the end of each batch
//					//if (i + 1 == MOTORIST_BATCH_SIZE) {
//					//	vPrintString("Average Speed Matrix updated.\r\n");
//
//					//	// Additional logging for traveling time matrix
//					//	if (batchCount % 2 == 0) {
//					//		vPrintString("Average Traveling Time Matrix updated.\r\n");
//					//	}
//					//	else {
//					//		vPrintString("Average Traveling Time Matrix updated.\r\n\n");
//					//	}
//
//					//	// Update traffic condition matrix
//					//	if (batchCount % 2 == 0) {
//					//		// uptTrvCondMatrix(src, dst);
//					//		vPrintString("Traffic Condition Matrix updated.\r\n\n");
//					//	}
//					//}
//				}
//				// Release the mutex after updating the matrices
//				xSemaphoreGive(xMutex);
//
//				//printTrafficCond();
//				//printAvgSpeed();
//				//printAvgTrvTime();
//			}
//		}
//		else {
//			vPrintString("Could not receive from the queue. \r\n");
//		}
//	}
//}
//
//int main(void) {
//	initAverageSpeedMatrix();
//	initTrafficMatrix();
//	initAverageTrvTimeMatrix();
//
//	// Create queue
//	queryQueue = xQueueCreate(10, sizeof(query_t));
//	mtSpeedQueue = xQueueCreate(1, sizeof(motorist_batch_t));
//
//	// Create semaphore
//	xMutex = xSemaphoreCreateMutex();
//
//	vPrintString("Main Menu:\n");
//	vPrintString("1. Dijkstra's Algorithm (Standard) - Evaluate Performance\n");
//	vPrintString("2. Dijkstra's Algorithm (Standard) - Check Accuracy\n");
//	vPrintString("3. Dijkstra's Algorithm (With Priority Queue) - Evaluate Performance\n");
//	vPrintString("4. Dijkstra's Algorithm (With Priority Queue) - Check Accuracy\n");
//	vPrintString("5. A* Algorithm - Evaluate Performance\n");
//	vPrintString("6. A* Algorithm - Check Accuracy\n");
//	vPrintString("7. Exit\n");
//	printf("\nEnter your choice: ");
//	scanf("%d", &sel);
//	printf("\n");
//
//	if (sel == 7) {
//		vPrintString("Program exiting...\n");
//		return 0;
//	}
//
//	// Create tasks
//	if (queryQueue != NULL && xMutex != NULL) {
//		xTaskCreate(vUserQueryTask, "User Query Task", 1024, NULL, 1, NULL);
//		xTaskCreate(vPathCalculationTask, "Path Calculation Task", 1024, NULL, 2, NULL);
//		xTaskCreate(vMotoristDataTask, "Motorist Task", 1024, NULL, 1, NULL);
//		xTaskCreate(vUpdateMatricesTask, "Matrices Update Task", 1024, NULL, 3, NULL);
//
//		vTaskStartScheduler();
//	}
//	else {
//		// Queue was not created
//	}
//
//	for (;;);
//
//	return 0;
//}