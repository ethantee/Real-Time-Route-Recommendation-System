#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "supporting_functions.h"
#include <semphr.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdbool.h>
#include <time.h>

#define N 11 // Number of malls
#define INF FLT_MAX // Infinity to represent no direct path
#define MAX_QUERIES 50000 // Maximum number of user quiries
#define MIN_SPEED 5 // Minimum speed in km/h
#define MAX_SPEED 140 // Maximum speed in km/h

// Queue management
QueueHandle_t queryQueue;

// Start time
volatile TickType_t xStartTime;

// User query generation struct
typedef struct {
	int srcNode; //Source node
	int dstNode; //Distance node
} query_t;

// Declare arrays for user queries and malls
query_t user_query[MAX_QUERIES];

typedef struct {
	double trfCondition; // Traffic Condition {1.0 <= x <= 2.0}
	double travelCost; // Travel Cost (Distance * Traffic Condition)
} weightedRoute_t;

// Mall names
const char* mallNames[N] = {
	"AMTM", "APBJ", "AKSM", "BSC", "BTS", "BMSC", "BBP", "DCM", "EP", "F88", "GDSM"
};

// Distance matrix (in km, INF = no direct path)
static float distanceMatrix[N][N] = {
	{INF, 14.5, 7.6, 13, 4, 15.2, INF, INF, 14.8, 5.4, 17.7},       // AMTM
	{14.6, INF, 17.4, 13.6, 14.4, 26.1, INF, INF, 5.3, INF, 14.5},  // APBJ
	{7.3, 22.1, INF, 9.6, INF, 10.9, INF, INF, 15.2, INF, 16.4},    // AKSM
	{11.2, 13.1, INF, INF, INF, 9.9, INF, 1.7, 16.4, INF, 5.5},     // BSC
	{3.4, INF, INF, 9.1, INF, 13, INF, INF, 12.5, INF, 15.9},       // BTS
	{17.4, 22.8, INF, 11.1, INF, INF, INF, INF, 22.7, INF, 12.2},   // BMSC
	{INF, INF, INF, 8, INF, 10.5, INF, INF, 12.6, INF, 12.7},       // BBP
	{11.5, 16.9, INF, INF, 8.5, 9.4, 9.3, INF, 18.4, INF, 6.2},     // DCM
	{11.5, 4, 14.9, 14.8, 11.9, 20.7, INF, INF, INF, INF, 16.7},    // EP
	{INF, INF, INF, INF, 0.85, INF, INF, INF, 13.3, INF, INF},      // F88
	{16.2, 17.3, 14, 6.6, INF, 13.1, INF, INF, 18.5, INF, INF}      // GDSM
};

// Traffic matrix 
weightedRoute_t trafficMatrix[N][N];

// Initialize traffic condition (traffic matrix) --- PASS THE PARAMETER WHEN USER GENERATE QUERY
void initTrafficConditionMatrix() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (distanceMatrix[i][j] != INF)
				trafficMatrix[i][j].trfCondition = 1.0;
			else
				trafficMatrix[i][j].trfCondition = INF;
		}
	}
}

// Initialize travel cost (traffic matrix) --- PASS THE PARAMETER WHEN USER GENERATE QUERY
void initTravelCostMatrix() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (distanceMatrix[i][j] != INF)
				trafficMatrix[i][j].travelCost = distanceMatrix[i][j];
			else
				trafficMatrix[i][j].travelCost = INF;
		}
	}
}

// Update traffic condition and travel cost (traffic matrix) --- PASS THEPARAMETER WHEN UPDATE TASK UPDATING
void uptTrfConditionAndTravelCost() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			// Update the traffic condition for representing congestion purpose
			if (distanceMatrix[i][j] != INF && trafficMatrix[i][j].trfCondition < 2.0) {
				trafficMatrix[i][j].trfCondition += 0.1;
			}
			if (distanceMatrix[i][j] != INF) {
				trafficMatrix[i][j].travelCost = distanceMatrix[i][j] * trafficMatrix[i][j].trfCondition;
			}
		}
	}
}	

// Dijkstra's Algorithm
void dijkstra(int src, float dist[], int prev[], weightedRoute_t graph[N][N]) {
	bool visited[N];

	for (int i = 0; i < N; i++) {
		dist[i] = INF;
		prev[i] = -1;
		visited[i] = false;
	}

	dist[src] = 0;

	// Find shortest path for all vertices
	for (int count = 0; count < N - 1; count++) {
		int u = -1;
		float minDist = INF;

		for (int v = 0; v < N; v++) {
			if (!visited[v] && dist[v] <= minDist) {
				minDist = dist[v];
				u = v;
			}
		}

		// Mark the picked vertex as visited
		visited[u] = true;

		// Update dist value of the adjacent vertices of the picked vertex
		for (int v = 0; v < N; v++) {
			if (!visited[v] && graph[u][v].travelCost != INF && dist[u] + graph[u][v].travelCost < dist[v]) {
				prev[v] = u;
				dist[v] = dist[u] + graph[u][v].travelCost;
			}
		}
	}
}

// Print shortest path
void printShortestPath(int prev[], int src, int vertex, const char* mallNames[]) {
	if (vertex == src) {
		printf("%s", mallNames[vertex]);
		return;
	}

	if (prev[vertex] == -1) {
		return INF;
	}

	printShortestPath(prev, src, prev[vertex], mallNames);
	printf(" -> %s", mallNames[vertex]);
}

// Calculate total distance to print the initial distance value to user
float totalDistanceCalculation(int prev[], int src, int vertex) {
	if (vertex == src) {
		return 0;
	}

	if (prev[vertex] == -1) {
		return INF;
	}

	return distanceMatrix[prev[vertex]][vertex] + totalDistanceCalculation(prev, src, prev[vertex]);
}

// User Query Generation Task (Sender Task)
static void vUserQueryTask(void* pvParameters) {
	srand((unsigned)time(NULL));
	query_t lValueToSend;
	BaseType_t xStatus;
	unsigned int queryCount = 1;

	// random generate source and destination query and user speed
	for (unsigned int i = 0; i < MAX_QUERIES; i++) {
		lValueToSend.srcNode = rand() % N;

		do {
			lValueToSend.dstNode = rand() % N;
		} while (lValueToSend.srcNode == lValueToSend.dstNode);

		// Print generated query and user speed
		printf("User %d\t\t: %s -> %s\n",
			queryCount++,
			mallNames[lValueToSend.srcNode],
			mallNames[lValueToSend.dstNode]);

		// Send query and speed data to the queue
		// 100 represents the maximum time the task should block waiting for space to become available
		// on the queue if the queue is already full
		xStatus = xQueueSendToBack(queryQueue, &lValueToSend, 100);

		if (xStatus != pdPASS) {
			vPrintString("Could not send to the queue. \r\n");
		}

		// Short delay of 100 milliseconds to allow pathCalculationTask (Receiver Task) to receive the queue
		//vTaskDelay(pdMS_TO_TICKS(100));
	}
	vTaskDelete(NULL);
}

// Path Calculation Task (Receiver Task)
static void vPathCalculationTask(void* pvParameters) {
	query_t	lReceivedValue;
	BaseType_t xStatus;
	float dist[N];
	int prev[N]; //store the previous vertex in the shortest path from the source to each vertex

	while (true) {
		xStatus = xQueueReceive(queryQueue, &lReceivedValue, portMAX_DELAY);

		if (xStatus == pdPASS) {
			dijkstra(lReceivedValue.srcNode, dist, prev, trafficMatrix);

			// Print shortest path and total distance
			printf("Shortest Path\t: ");
			printShortestPath(prev, lReceivedValue.srcNode, lReceivedValue.dstNode, mallNames);
			float totalDistance = totalDistanceCalculation(prev, lReceivedValue.srcNode, lReceivedValue.dstNode);
			printf("\nDistance\t: %.2f km\n\n", totalDistance);
		}
		else {
			vPrintString("Could not receive from the queue. \r\n");
		}
	}
}

int main(void) {
	initTrafficConditionMatrix(); 
	initTravelCostMatrix();

	queryQueue = xQueueCreate(20, sizeof(query_t));

	if (queryQueue != NULL) {
		xTaskCreate(vUserQueryTask, "User Query Task", 1024, NULL, 1, NULL);
		xTaskCreate(vPathCalculationTask, "Path Calculation", 1024, NULL, 2, NULL);

		vTaskStartScheduler();
	}
	else {
		// Queue was not created
	}

	for (;;);

	return 0;
}