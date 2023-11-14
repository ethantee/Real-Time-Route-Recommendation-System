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

#define N 11				// number of malls
#define INF FLT_MAX			// infinity in distances
#define MAX_QUERIES 1000	// maximum number of user queries
#define MIN_SPEED 5			// minimum speed in km/h
#define MAX_SPEED 140		// maximum speed in km/h

// Queue and semaphore handles
QueueHandle_t xQueue;
SemaphoreHandle_t xSemaphore;

// Start time
volatile TickType_t xStartTime;

// User query structure
typedef struct {
	int src;
	int dst;
} query_t;

// Mall structure
typedef struct {
	int vertex;
	float dist;
	int prev;
} mall_t;

// Declare arrays for user queries and malls
query_t user_query[MAX_QUERIES];
mall_t malls[N];

// Mall names
const char* mallNames[N] = {
	"AMTM", "APBJ", "AKSM", "BSC", "BTS", "BMSC", "BBP", "DCM", "EP", "F88", "GDSM"
};

// Distance matrix (in km, INF = not connected) - static
float distanceMatrix[N][N] = {
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

// Query count matrix - dynamic
int queryCountMatrix[N][N] = { 0 };

// Traffic matrix (1 = best, 2 = worst) - static
double trafficMatrix[N][N];

// Weight matrix (distance * traffic) - dynamic
double weightMatrix[N][N];

// Speed range (km/h) - static
float speedRange[11][2] = {
	{1.0, 140.0},
	{1.1, 126.5},
	{1.2, 113.0},
	{1.3, 99.5},
	{1.4, 86.0},
	{1.5, 72.5},
	{1.6, 59.0},
	{1.7, 45.5},
	{1.8, 32.0},
	{1.9, 18.5},
	{2.0, 5.0}
};

// Initialise traffic matrix
void initTrafficMatrix() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (distanceMatrix[i][j] != INF)
				trafficMatrix[i][j] = 1.0;
			else
				trafficMatrix[i][j] = INF;
		}
	}
}

// Initialise weight matrix
void initWeightMatrix() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (distanceMatrix[i][j] != INF)
				weightMatrix[i][j] = distanceMatrix[i][j];
			else
				weightMatrix[i][j] = INF;
		}
	}
}

// Update traffic matrix
void updateTrafficMatrix(int src, int dst) {
	queryCountMatrix[src][dst]++;

	if (queryCountMatrix[src][dst] % 10 == 0 && trafficMatrix[src][dst] < 2.0) {
		trafficMatrix[src][dst] += 0.1;
	}
}

// Print Query Count Matrix
void printQueryCountMatrix() {
	printf("Query Count Matrix:\n");
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			printf("%d\t", queryCountMatrix[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

// Print Traffic Matrix
void printTrafficMatrix() {
	printf("Traffic Matrix:\n");
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (trafficMatrix[i][j] == INF)
				printf("INF\t");
			else
				printf("%.1f\t", trafficMatrix[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

// Dijkstra's Algorithm
void dijkstra(int src, float dist[], int prev[], float graph[N][N]) {
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
			if (!visited[v] && graph[u][v] != INF && dist[u] + graph[u][v] < dist[v]) {
				prev[v] = u;
				dist[v] = dist[u] + graph[u][v];
			}
		}
	}
}

void printPath(int prev[], int src, int vertex, const char* mallNames[]) {
	if (vertex == src) {
		printf("%s", mallNames[vertex]);
		return;
	}

	if (prev[vertex] == -1) {
		printf("No path exists");
		return;
	}

	printPath(prev, src, prev[vertex], mallNames);
	printf(" -> %s", mallNames[vertex]);
}

static void vSenderTask(void* pvParameters) {
	srand((unsigned)time(NULL));
	query_t lValueToSend;
	BaseType_t xStatus;

	xStartTime = xTaskGetTickCount();

	for (unsigned int i = 0; i < MAX_QUERIES; i++) {
		if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
			lValueToSend.src = rand() % N;
			do {
				lValueToSend.dst = rand() % N;
			} while (lValueToSend.src == lValueToSend.dst);

			// Send query to the queue
			xStatus = xQueueSendToBack(xQueue, &lValueToSend, 100);

			if (xStatus != pdPASS) {
				vPrintString("Could not send to the queue.\r\n");
			}

			if (i == MAX_QUERIES - 1) {
				TickType_t xEndTime = xTaskGetTickCount();
				TickType_t xElapsedTime = xEndTime - xStartTime;
				printf("\nElapsed time: %u ms\n", (xElapsedTime * portTICK_PERIOD_MS));
				printTrafficMatrix();
				printQueryCountMatrix();
			}
		}
		xSemaphoreGive(xSemaphore);
	}
	vTaskDelete(NULL);
}

// Function to receive user queries
static void vReceiverTask(void* pvParameters) {
	query_t lReceivedValue;
	BaseType_t xStatus;
	unsigned int queryCount = 0;
	float dist[N];
	int prev[N];

	while (true) {
		xStatus = xQueueReceive(xQueue, &lReceivedValue, portMAX_DELAY);

		if (xStatus == pdPASS) {
			dijkstra(lReceivedValue.src, dist, prev, distanceMatrix);
			queryCount++;

			// Print output
			printf("User %05d\t: %s -> %s\n",
				queryCount,
				mallNames[lReceivedValue.src],
				mallNames[lReceivedValue.dst]);
			printf("Shortest Path\t: ");
			printPath(prev, lReceivedValue.src, lReceivedValue.dst, mallNames);
			printf("\nDistance\t: %.2f km\n\n",
				dist[lReceivedValue.dst]);

			// Update traffic matrix
			xSemaphoreTake(xSemaphore, portMAX_DELAY);
			int currentVertex = lReceivedValue.dst;
			while (currentVertex != lReceivedValue.src && prev[currentVertex] != -1) {
				updateTrafficMatrix(prev[currentVertex], currentVertex);
				currentVertex = prev[currentVertex];
			}
			xSemaphoreGive(xSemaphore);
		}
		else {
			vPrintString("Could not receive from the queue.\r\n");
		}
	}
}

int main(void) {
	initTrafficMatrix();
	initWeightMatrix();

	printTrafficMatrix();

	xSemaphore = xSemaphoreCreateBinary();
	xSemaphoreGive(xSemaphore);

	xQueue = xQueueCreate(20, sizeof(query_t));

	if (xQueue != NULL) {
		xTaskCreate(vSenderTask, "Sender", 1024, NULL, 1, NULL);
		xTaskCreate(vReceiverTask, "Receiver", 1024, NULL, 2, NULL);

		vTaskStartScheduler();
	}
	else {
		// Queue was not created
	}

	for (;; );

	return 0;
}