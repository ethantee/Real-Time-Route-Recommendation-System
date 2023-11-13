//#include "FreeRTOS.h"
//#include "task.h"
//#include "queue.h"
//#include "semphr.h"
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
//// Queue and semaphore handles
//QueueHandle_t xQueue;
//SemaphoreHandle_t xSemaphore;
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
//// Distance matrix (in km, INF = not connected) - static
//float distanceMatrix[N][N] = {
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
//// Traffic matrix (1 = best, 2 = worst) - static
//double trafficMatrix[N][N];
//
//// Weight matrix (distance * traffic) - dynamic
//double weightMatrix[N][N];
//
//// Query matrix (number of users) - dynamic
//int queryMatrix[N][N] = { 0 };
//
//// Initialise traffic matrix
//void initTrafficMatrix() {
//	for (int i = 0; i < N; i++) {
//		for (int j = 0; j < N; j++) {
//			if (distanceMatrix[i][j] != INF)
//				trafficMatrix[i][j] = 1.0;
//			else
//				trafficMatrix[i][j] = INF;
//		}
//	}
//}
//
//// Initialise weight matrix
//void initWeightMatrix() {
//	for (int i = 0; i < N; i++) {
//		for (int j = 0; j < N; j++) {
//			if (distanceMatrix[i][j] != INF)
//				weightMatrix[i][j] = distanceMatrix[i][j];
//			else
//				weightMatrix[i][j] = INF;
//		}
//	}
//}
//
//// Print Traffic Matrix
//void printTrafficMatrix() {
//	printf("Traffic Matrix:\n");
//	for (int i = 0; i < N; i++) {
//		for (int j = 0; j < N; j++) {
//			if (trafficMatrix[i][j] == INF) {
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
//// Dijkstra's Algorithm
//void dijkstra() {}
//
//
//static void vSenderTask(void* pvParameters) {
//	query_t queryToSend;
//	BaseType_t xStatus;
//
//	for (unsigned int i = 0; i < MAX_QUERIES; i++) {
//		queryToSend.src = rand() % N;
//		do {
//			queryToSend.dst = rand() % N;
//		} while (queryToSend.src == queryToSend.dst);
//
//		// Send query to the queue
//		xStatus = xQueueSendToBack(xQueue, &queryToSend, portMAX_DELAY);
//
//		if (xStatus != pdPASS) {
//			vPrintString("Could not send to the queue.\r\n");
//		}
//
//		vTaskDelay(10);
//	}
//	vTaskDelete(NULL);
//}
//
//// Function to receive user queries
//static void vReceiverTask(void* pvParameters) {
//	query_t lReceivedValue;
//	BaseType_t xStatus;
//	unsigned int queryCount = 0;
//
//	for (;;) {
//		xStatus = xQueueReceive(xQueue, &lReceivedValue, portMAX_DELAY);
//
//		if (xStatus == pdPASS) {
//			queryCount++;
//			printf("User %3d\t: %s -> %s\n", queryCount, mallNames[lReceivedValue.src], mallNames[lReceivedValue.dst]);
//		}
//	}
//}
//
//int main(void) {
//	xQueue = xQueueCreate(1000, sizeof(query_t));
//	xSemaphore = xSemaphoreCreateBinary();
//	
//	xSemaphoreGive(xSemaphore);
//
//	if (xQueue != NULL) {
//		xTaskCreate(vSenderTask, "Sender", 1024, NULL, 1, NULL);
//		xTaskCreate(vReceiverTask, "Receiver", 1024, NULL, 2, NULL);
//
//		vTaskStartScheduler();
//	}
//	else {
//		// Queue was not created
//	}
//
//	for (;; );
//
//	return 0;
//}