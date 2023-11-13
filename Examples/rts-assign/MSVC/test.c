//#include "FreeRTOS.h"
//#include "task.h"
//#include "queue.h"
//#include "supporting_functions.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>
//#include <float.h>
//#include <stdbool.h>
//#include <time.h>
//
//#define N 11				// number of malls
//#define INF FLT_MAX			// infinity in distances
//#define MAX_QUERIES 1000	// maximum number of user queries
//#define MIN_SPEED 5			// minimum speed in km/h
//#define MAX_SPEED 140		// maximum speed in km/h
//
//// Queue handle
//QueueHandle_t xQueue;
//
//// User query structure
//typedef struct {
//	int src;
//	int dst;
//} query_t;
//
//// Mall structure
//typedef struct {
//	int id;
//	float dist;
//	int prev;
//} mall_t;
//
//// Declare arrays for user queries and malls
//query_t user_query[MAX_QUERIES];
//mall_t malls[N];
//
//// Mall names
//const char* mallNames[N] = {
//	"AMTM", "APBJ", "AKSM", "BSC", "BTS", "BMSC", "BBP", "DCM", "EP", "F88", "GDSM"
//};
//
//// Distance adjacency matrix (in km, INF = not connected) - static
//float distanceMatrix[11][11] = {
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
//// Traffic adjacency matrix (1 = best, 2 = worst) - static
//double trafficMatrix[N][N];
//
//// Weight adjacency matrix (distance * traffic) - dynamic
//double weightMatrix[N][N];
//
//// Initialise traffic matrix
//void initTrafficMatrix() {
//	for (int i = 0; i < N; i++) {
//		for (int j = 0; j < N; j++) {
//			if (distanceMatrix[i][j] != INF) {
//				trafficMatrix[i][j] = 1.0;
//			}
//			else {
//				trafficMatrix[i][j] = -1;
//			}
//		}
//	}
//}
//
//// Initialise weight matrix
//void initWeightMatrix() {
//	for (int i = 0; i < N; i++) {
//		for (int j = 0; j < N; j++) {
//			if (distanceMatrix[i][j] != INF) {
//				weightMatrix[i][j] = distanceMatrix[i][j];
//			}
//			else {
//				weightMatrix[i][j] = INF;
//			}
//		}
//	}
//}
//
//// Update traffic matrix
//void updateTrafficMatrix(int path[], int pathLength) {
//	for (int i = 0; i < pathLength - 1; i++) {
//		int from = path[i];
//		int to = path[i + 1];
//		if (from >= 0 && from < N && to >= 0 && to < N) {
//			trafficMatrix[from][to] *= 1.1;
//			if (trafficMatrix[from][to] > 2.0) {
//				trafficMatrix[from][to] = 2.0;
//			}
//		}
//	}
//}
//
//// Update weight matrix
//void updateWeightMatrix() {
//	for (int i = 0; i < N; i++) {
//		for (int j = 0; j < N; j++) {
//			if (trafficMatrix[i][j] != -1) {
//				weightMatrix[i][j] = distanceMatrix[i][j] * trafficMatrix[i][j];
//			}
//			else {
//				weightMatrix[i][j] = INF;
//			}
//		}
//	}
//}
//
//// Print traffic matrix
//void printTrafficMatrix() {
//	printf("Traffic Matrix:\n");
//	for (int i = 0; i < N; i++) {
//		for (int j = 0; j < N; j++) {
//			if (trafficMatrix[i][j] == -1) {
//				printf("INF\t");
//			}
//			else {
//				printf("%.1f\t", trafficMatrix[i][j]);
//			}
//		}
//		printf("\n");
//	}
//	printf("\n");
//}
//
//// Generate user queries
//void generateQueries() {
//	for (int i = 0; i < MAX_QUERIES; i++) {
//		user_query[i].src = rand() % N;
//		do {
//			user_query[i].dst = rand() % N;
//		} while (user_query[i].src == user_query[i].dst);
//	}
//}
//
//// Generate random speed
//int generateRandomSpeed() {
//	return MIN_SPEED + (rand() % (MAX_SPEED - MIN_SPEED + 1));
//}
//
//// Dijkstra's Algorithm
//void dijkstra(int src, int dst, mall_t malls[N], int path[N], int* pathLength, float* totalDistance) {
//	updateWeightMatrix();
//
//	int visited[N] = { 0 };
//
//	// Initialize nodes
//	for (int i = 0; i < N; i++) {
//		malls[i].id = i;
//		malls[i].dist = INF;
//		malls[i].prev = -1;
//	}
//
//	malls[src].dist = 0;
//
//	for (int count = 0; count < N - 1; count++) {
//		// Pick the minimum distance node from the set of nodes not yet processed
//		int u = -1;
//		float minDist = INF;
//		for (int i = 0; i < N; i++) {
//			if (!visited[i] && malls[i].dist <= minDist) {
//				minDist = malls[i].dist;
//				u = i;
//			}
//		}
//
//		// If no unvisited node is found, break the loop
//		if (u == -1) {
//			break;
//		}
//
//		// Mark the picked node as processed
//		visited[u] = 1;
//
//		// Update dist value of the adjacent nodes of the picked node
//		for (int v = 0; v < N; v++) {
//			if (!visited[v] && weightMatrix[u][v] != INF && malls[u].dist + weightMatrix[u][v] < malls[v].dist) {
//				malls[v].dist = malls[u].dist + weightMatrix[u][v];
//				malls[v].prev = u;
//			}
//		}
//	}
//
//	// Reconstruct the shortest path from source to target
//	int stack[N];
//	int stackSize = 0;
//	int u = dst;
//
//	while (malls[u].prev != -1) {
//		stack[stackSize++] = u;
//		u = malls[u].prev;
//	}
//
//	// If the target is reachable from the source
//	if (malls[dst].dist != INF) {
//		stack[stackSize++] = src;
//		*pathLength = 0;
//		// Reverse path stack to get correct order
//		while (stackSize) {
//			path[(*pathLength)++] = stack[--stackSize];
//		}
//		*totalDistance = malls[dst].dist;
//	}
//}
//
//// Function to generate user queries
//static void vSenderTask(void* pvParameters) {
//	query_t lValueToSend;
//	BaseType_t xStatus;
//	size_t i = 0;
//
//	while (1) {
//		lValueToSend = user_query[i % MAX_QUERIES];
//		xStatus = xQueueSendToBack(xQueue, &lValueToSend, 0);
//
//		if (xStatus != pdPASS) {
//			vPrintString("Could not send to the queue.\r\n");
//		}
//
//		i = (i + 1) % MAX_QUERIES;
//
//		vTaskDelay(pdMS_TO_TICKS(10));
//	}
//}
//
//// Function to receive user queries
//static void vReceiverTask(void* pvParameters) {
//	query_t lReceivedValue;
//	BaseType_t xStatus;
//	unsigned int userID = 0;
//
//	// Block for 10ms
//	const TickType_t xTicksToWait = pdMS_TO_TICKS(10);
//
//	for (;; ) {
//		if (_kbhit()) {
//			char c = _getch();
//			vTaskEndScheduler();
//		}
//
//		xStatus = xQueueReceive(xQueue, &lReceivedValue, xTicksToWait);
//
//		if (xStatus == pdPASS) {
//			int totalQueries = 0;
//
//			while (totalQueries < 100) {
//				int src = rand() % N;
//				int dst = rand() % N;
//				while (src == dst) {
//					dst = rand() % N;
//				}
//
//				int speed = generateRandomSpeed();
//				printf("User %d\t\t: %d -> %d\n", totalQueries + 1, src, dst);
//
//				mall_t nodes[N];
//				int path[N];
//				int pathLength = 0;
//				float totalDistance = 0;
//				float totalWeightedDistance = 0;
//
//				// Make sure to update weight matrix before finding shortest path
//				updateWeightMatrix();
//				dijkstra(src, dst, nodes, path, &pathLength, &totalWeightedDistance);
//
//				// Calculate travelling time based on the total distance
//				float travellingTimeMinutes = 0;
//				for (int i = 0; i < pathLength - 1; i++) {
//					int from = path[i];
//					int to = path[i + 1];
//					totalDistance += distanceMatrix[from][to];
//				}
//				travellingTimeMinutes = (totalDistance / speed) * 60;
//
//				// Calculate real time based on the total weighted distance
//				float realTimeMinutes = (totalWeightedDistance / speed) * 60;
//
//				// Print the shortest path
//				printf("Shortest Path\t: ");
//				for (int i = 0; i < pathLength; i++) {
//					printf("%d", path[i]);
//					if (i < pathLength - 1) {
//						printf(" -> ");
//					}
//				}
//
//				printf("\nSpeed\t\t: %d km/h\n", speed);
//				printf("Distance\t: %.2f km\n", totalDistance);
//				printf("Travelling Time\t: %.2f minute(s)\n", travellingTimeMinutes);
//				printf("Real Time\t: %.2f minute(s)\n\n", realTimeMinutes);
//
//				// Update traffic matrix only if a path is found
//				if (pathLength > 0) {
//					updateTrafficMatrix(path, pathLength);
//				}
//
//				totalQueries++;
//				vTaskDelay(10);
//			}
//		}
//		else {
//			vPrintString("Could not receive from the queue.\r\n");
//		}
//	}
//}
//
//int main() {
//	// Initialise random seed
//	srand(time(NULL));
//
//	xQueue = xQueueCreate(1000, sizeof(query_t));
//
//	if (xQueue != NULL) {
//		xTaskCreate(vSenderTask, "Sender", 1024, NULL, 2, NULL);
//		xTaskCreate(vReceiverTask, "Receiver", 1024, NULL, 1, NULL);
//		vTaskStartScheduler();
//	}
//	else {
//		vPrintString("Queue creation failed.\r\n");
//	}
//
//	return 0;
//}