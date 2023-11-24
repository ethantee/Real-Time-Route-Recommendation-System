//#include "FreeRTOS.h"
//#include "queue.h"
//#include "supporting_functions.h"
//#include "task.h"
//#include <float.h>
//#include <math.h>
//#include <semphr.h>
//#include <stdbool.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>
//
//#define N 44						// Number of malls
//#define INF FLT_MAX					// Infinity to represent no direct path
//#define MAX_QUERIES 50000			// Number of user quiries
//#define SRC_NODE rand() % N			// Randomize source node
//#define DST_NODE rand() % N			// Randomize destination node
//#define MOTORIST_BATCH_SIZE 5000	// Number of motorists in one batch
//#define MIN_TRFCONDITION 0			// Minimum traffic congestion index
//#define MAX_TRFCONDITION 10			// Maximum traffic congestion index
//
//// User selection
//int sel = 0;
//
//// Queue management
//QueueHandle_t queryQueue = NULL;
//QueueHandle_t mtSpeedQueue = NULL;
//
//// Resource management
//SemaphoreHandle_t xMutex = NULL;
//
//// Timing management
//volatile TickType_t xStartTime = 0;
//TickType_t queryStartTimes[MAX_QUERIES] = { 0 };
//TickType_t queryEndTimes[MAX_QUERIES] = { 0 };
//
//// Notification management
//TaskHandle_t xMotoristDataTaskHandle = NULL;
//TaskHandle_t xUpdateMatricesTaskHandle = NULL;
//
//// User query generation struct
//typedef struct {
//	int srcNode;	// Source node
//	int dstNode;	// Distance node
//} query_t;
//
//// Motorists path and speed generation struct
//typedef struct {
//	int srcNode;	// Source node
//	int dstNode;	// Distance node
//	double mtSpeed; // Motorist speed
//} motorist_t;
//
//// Motorist batch struct
//typedef struct {
//	motorist_t motorists[MOTORIST_BATCH_SIZE]; // A batch holds an defined amount of motorist
//} motorist_batch_t;
//
//// Traffic matrix's struct data
//typedef struct {
//	double trfCondition;	// Traffic condition {Congestion index: 0 <= x <= 10}
//} trafficRoute_t;
//
//// Average speed matrix's struct data
//typedef struct {
//	double pathCount;	// Number of motorist 
//	double accSpeed;	// Accumulated speed of motorist
//	double avgSpeed;	// Average speed (Accumulated Speed / Number of motorist)
//} speedRoute_t;
//
//// Average time matrix's struct data
//typedef struct {
//	double avgTrvTime;	// Average travelling time (Distance / Average Speed)
//} timeRoute_t;
//
//// Mall names
//const char* mallNames[N] = {
//	"AMTM", "APBJ", "AKSM", "BSC", "BTS", "BMSC", "BBP", "DCM", "EP", "F88", "GDSM",
//	"GEM", "HSC", "IC", "IPKL", "IM", "KFW", "IKEC", "KGM", "L10", "LK", "MSM",
//	"MVM", "LBBCC", "MSC", "NSSM", "PBJ", "PDH", "PKL", "PC", "PLY", "PSP", "PPSM",
//	"QCMKL", "SCM", "SKL", "TS", "SWP", "SPM", "SVM", "SK", "TGM", "TLK", "TW"
//};
//
//// Distance matrix (in km, INF = no direct path) - Static
//static float distanceMatrix[N][N] = {
//  {INF, 14.5, 7.6, 13.0, 4.0, 15.2, INF, INF, 14.8, 5.4, 17.7, 6.5, 14.7, INF, INF, 6.7, 3.6, 11.5, 11.7, INF, 20.3, 17.2, 11.0, 3.7, 1.6, 10.3, INF, 12.0, INF, INF, INF, 9.9, 3.2, INF, 14.3, 8.7, 5.6, 4.1, 10.6, 1.2, 6.8, 10.2, 5.8, INF},						// AMTM
//  {14.6, INF, 17.4, 13.6, 14.4, 26.1, INF, INF, 5.3, INF, 14.5, 17.5, 19.0, INF, 13.9, 16.5, 16.8, 10.5, 11.1, INF, 23.3, 24.6, 10.0, INF, 13.6, 10.6, 1.0, 16.3, 16.4, 14.4, INF, 10.0, 14.1, 20.3, 23.2, 14.2, 15.2, INF, 19.5, 13.0, 16.6, INF, 15.4, INF},		// APBJ
//  {7.3, 22.1, INF, 9.6, INF, 10.9, INF, INF, 15.2, INF, 16.4, 3.4, 10.5, INF, INF, 1.3, 4.7, 13.2, 13.5, INF, 13.5, 10.2, 12.1, INF, INF, 10.5, INF, 9.5, 2.3, 2.8, INF, INF, INF, 2.7, 8.3, INF, INF, INF, 5.4, INF, 1.0, 8.9, 1.0, 2.2},							// AKSM
//  {11.2, 13.1, INF, INF, INF, 9.9, INF, 1.7, 16.4, INF, 5.5, 14.9, 5.7, INF, INF, 11.3, 9.8, 4.7, 4.3, INF, 11.5, 15.0, 4.2, INF, INF, 4.3, INF, 2.0, 8.3, 7.2, INF, 11.7, 9.3, 8.8, 13.1, 7.0, INF, INF, 8.5, 11.0, 10.3, 3.4, 11.7, 7.4},							// BSC
//  {3.4, INF, INF, 9.1, INF, 13.0, INF, INF, 12.5, INF, 15.9, 7.1, 10.8, INF, INF, 6.0, 1.2, 7.8, 7.9, INF, 15.2, 14.9, 7.2, INF, 4.0, 5.4, INF, 8.2, INF, INF, INF, 8.4, 1.3, INF, 14.1, INF, INF, INF, INF, 2.7, INF, 7.6, INF, 3.9},								// BTS
//  {17.4, 22.8, INF, 11.1, INF, INF, INF, INF, 22.7, INF, 12.2, 13.8, 7.2, INF, 12.4, 10.8, 15.7, 13.4, 14.8, INF, 5.6, 10.6, 12.8, INF, 15.3, 12.2, INF, 10.2, 10.5, 8.5, INF, 18.7, 14.4, 7.6, 10.5, 9.7, INF, INF, 6.9, 15.3, INF, 12.1, INF, 9.8},				// BMSC
//  {INF, INF, INF, 8.0, INF, 10.5, INF, INF, 12.6, INF, 12.7, 6.7, 9.7, INF, 0.3, 3.7, INF, 7.6, 8.0, INF, 13.7, 13.4, 7.1, INF, 2.7, 5.4, INF, 7.1, INF, 3.7, INF, 8.6, 1.7, 4.2, 11.4, 3.6, INF, INF, 6.0, 3.2, INF, 6.3, INF, 1.5},								// BBP
//  {11.5, 16.9, INF, INF, 8.5, 9.4, 9.3, INF, 18.4, INF, 6.2, INF, 6.3, INF, INF, 10.5, 8.8, 6.8, 5.6, INF, 10.7, 13.8, 6.2, INF, INF, 6.4, INF, 0.1, 7.6, 6.0, INF, 13.3, 8.5, 7.6, 11.9, 5.9, 8.0, INF, 7.3, 11.4, INF, 4.9, INF, 6.8},							// DCM
//  {11.5, 4.0, 14.9, 14.8, 11.9, 20.7, INF, INF, INF, INF, 16.7, 15.7, 17.6, INF, INF, 14.3, 11.2, 10.7, 12.2, INF, 22.8, 26.4, 10.1, 11.2, 11.2, 11.2, INF, 17.2, 13.6, 14.1, INF, 4.6, 10.9, 18.0, 20.9, 14.0, INF, 11.8, 18.1, 10.7, 14.3, 9.4, 13.3, INF},		// EP
//  {INF, INF, INF, INF, 0.85, INF, INF, INF, 13.3, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, 0.094, INF, 12.4, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, 1.6, INF, INF, INF, INF, INF, INF, 3.7, INF, INF, INF, INF},								// F88
//  {16.2, 17.3, 14.0, 6.6, INF, 13.1, INF, INF, 18.5, INF, INF, 17.8, 7.6, 15.3, INF, 15.2, 14.5, 8.5, 7.6, INF, 10.8, 19.7, 8.2, INF, 15.3, 10.5, 15.5, 6.0, INF, 11.1, INF, 13.8, 13.2, 12.7, 17.0, 10.9, INF, INF, 12.4, 16.1, 12.9, 9.6, INF, 11.8},				// GDSM
//  {5.3, 21.6, 3.9, 13.1, 5.8, 12.5, INF, INF, 18.1, INF, 18.1, INF, 11.8, 5.4, INF, 2.8, INF, 12.9, 13.3, INF, 15.2, 8.6, 12.4, INF, 5.1, 10.8, INF, 12.2, 4.9, 5.5, INF, 12.2, 5.7, 5.6, 7.8, INF, 5.3, 5.7, 6.9, 6.3, 3.3, 12.7, 2.4, 4.9},						// GEM
//  {14.0, 16.8, INF, 7.4, 11.0, 7.0, INF, INF, 19.3, INF, 7.7, 11.7, INF, INF, INF, INF, 11.3, 9.3, 10.6, INF, 8.4, 13.6, 10.5, INF, 12.6, 8.8, INF, 5.2, 8.8, 6.8, INF, 15.9, 10.1, 7.0, 10.9, 7.9, 9.1, INF, 6.3, 13.6, 8.3, 9.7, INF, INF},						// HSC
//  {1.7, 14.7, 5.9, 10.4, 2.7, INF, INF, INF, 11.8, INF, 17.2, 5.9, 14.2, INF, INF, 8.9, 2.7, 11.0, 14.6, INF, 17.8, 18.1, 10.4, 2.7, INF, 7.5, INF, 9.4, INF, 5.8, INF, 8.4, 1.7, 6.3, 15.3, 5.6, 3.6, 3.3, INF, 1.3, INF, 9.6, 7.3, INF},							// IC
//  {4.0, 16.1, INF, INF, INF, INF, INF, INF, 13.1, INF, INF, 6.3, INF, 3.3, INF, INF, INF, INF, 11.4, INF, INF, 14.1, INF, INF, 3.1, INF, INF, INF, INF, INF, INF, 9.0, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF},									// IPKL
//  {INF, 17.5, 1.7, 10.9, 3.4, 10.1, INF, INF, 14.7, INF, 19.4, 2.6, 9.7, INF, INF, INF, INF, 12.3, 12.7, INF, 12.8, 9.4, 11.7, INF, 3.5, 10.1, INF, 10.0, 2.4, 3.4, INF, 10.5, 4.2, 3.2, 7.5, INF, INF, 3.3, 4.5, 4.8, 1.2, 10.9, INF, 3.4},						// IM
//  {INF, 14.7, 5.8, 9.5, INF, 12.0, INF, INF, 11.5, INF, 14.8, 7.0, 11.8, 2.0, 3.3, 6.1, INF, 8.6, 10.0, INF, 15.5, 14.7, 8.1, 2.1, INF, 7.4, INF, 8.6, 5.6, 5.2, INF, 7.4, 1.4, 7.5, 13.5, 5.1, 4.7, INF, 8.9, 2.5, 5.4, 7.3, 5.1, 4.3},							// KFW
//  {10.2, 9.7, 9.6, 4.0, 7.9, 12.9, INF, INF, 11.6, INF, INF, 15.8, 10.2, 9.5, 7.6, 10.9, 8.5, INF, 1.9, INF, 14.3, 15.5, 2.0, INF, INF, 3.5, 10.7, 5.8, 7.5, 6.6, INF, 7.0, 8.9, 8.2, 13.6, 6.4, INF, INF, 9.0, 10.1, 9.5, INF, 11.3, 6.6},							// IKEC
//  {11.8, 9.6, 11.0, 5.0, INF, 13.5, INF, INF, 11.5, INF, 7.2, 17.4, 8.1, 10.9, 9.0, 13.3, 9.2, 3.4, INF, INF, 14.9, 17.0, 3.6, INF, INF, 4.9, 10.6, 6.4, 9.0, 8.1, INF, 8.6, 10.3, 9.7, 15.1, 7.9, INF, INF, 10.4, 11.7, 11.0, 4.6, 13.5, 8.1},						// KGM
//  {4.5, 16.4, 2.1, 8.1, 0.6, 9.5, 0.21, INF, 13.4, 0.5, 15.1, 5.8, 9.8, INF, INF, 4.0, INF, 12.3, 11.7, INF, 12.4, 12.6, 11.7, INF, 3.4, INF, 0.8, 9.5, 0.8, 2.7, INF, 9.3, 2.4, 3.3, 10.7, INF, 0.7, INF, 7.9, INF, 1.7, 6.7, 3.1, 0.85},							// L10
//  {19.8, 28.7, 15.2, 13.5, 17.2, 7.7, INF, INF, 25.8, INF, 10.6, 17.7, 10.6, 18.9, 17.0, 15.5, 18.3, 18.3, 17.2, INF, INF, 14.3, 18.0, INF, 19.4, 14.6, 15.6, 12.5, 15.6, 13.9, INF, 21.7, 16.8, 13.8, 14.7, 14.5, 18.6, INF, 13.1, 19.7, 15.1, 15.5, INF, 15.4},	// LK
//  {17.1, 30.2, 11.9, 16.9, 14.6, 10.8, INF, INF, 24.8, INF, 19.0, 9.1, 14.0, INF, 13.5, 9.8, 15.4, 17.2, 17.6, INF, 13.5, INF, 16.6, INF, 13.7, 15.0, INF, 16.0, 12.5, 9.1, INF, 20.7, 14.4, 9.0, 3.2, 11.8, 12.9, 13.5, 9.4, 15.0, 11.1, 15.8, 10.1, 10.5},		// MSM
//  {11.0, 10.5, 11.1, 4.7, 8.4, 15.7, INF, INF, 13.2, INF, 9.4, 16.6, 10.2, 10.4, 8.2, 13.8, 8.7, 2.1, 2.8, INF, 17.1, 18.3, INF, 10.8, 13.7, 4.3, INF, 8.6, 13.7, 8.1, INF, 10.2, 9.0, 9.7, 16.4, 7.9, 12.3, INF, 11.8, 10.9, 13.9, 1.1, 12.9, 9.5},				// MVM
//  {6.2, 16.3, 7.7, 8.7, 4.1, 13.5, INF, INF, 12.3, 5.1, 14.2, 10.8, 10.5, 3.1, 3.7, 6.8, 1.1, 7.4, 7.5, INF, 14.9, 21.5, 6.9, INF, INF, 5.1, INF, 7.8, 6.7, 6.7, INF, 8.1, 2.4, 8.3, 13.5, 6.6, 5.5, 3.8, 9.6, 3.5, 6.9, 6.8, 5.9, 2.4},							// LBBCC
//  {2.4, 15.0, 5.3, 9.6, 2.1, 12.1, INF, INF, 11.6, 3.3, 19.8, 5.3, 11.3, 0.23, 2.2, 4.4, 2.2, 9.6, 13.2, INF, 16.4, 13.1, 9.1, 2.2, INF, 8.7, INF, 8.6, 4.3, 5.5, INF, 7.8, 1.2, 8.2, 12.3, 5.3, 3.1, 2.1, 7.6, 1.7, 4.5, 10.4, 3.5, 4.4},							// MSC
//  {7.0, 12.8, 5.2, 6.1, 4.3, 12.9, INF, INF, 15.3, 5.4, 12.9, 13.1, 9.6, 5.4, 3.7, 6.6, 4.5, 5.6, 6.2, INF, 15.8, 15.5, 5.0, 5.5, 6.8, INF, INF, 7.0, INF, 7.5, INF, 10.8, 4.4, 4.5, 13.4, 4.3, 5.6, INF, 6.5, 7.4, 4.5, 4.2, 5.9, 3.4},							// NSSM
//  {16.1, 1.2, 19.5, 19.4, 16.8, 26.0, INF, INF, 6.5, INF, 21.4, 17.8, 22.2, 15.5, 16.8, 18.9, 16.0, 15.3, 14.7, INF, 29.0, 36.7, 14.7, 15.8, 14.0, 15.3, INF, 20.5, 16.7, 18.7, INF, 9.2, 17.0, 22.7, 33.8, 18.6, 18.0, 16.8, 22.1, 16.0, 18.9, 12.0, 17.9, INF},	// PBJ
//  {INF, INF, INF, INF, INF, INF, INF, 0.15, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF},									// PDH
//  {5.1, 16.9, 4.6, INF, 1.3, 13.7, INF, INF, 14.5, INF, 15.8, 5.6, 16.2, 3.8, 1.3, 3.7, 2.5, 12.9, 12.3, INF, 15.6, 12.3, 12.3, INF, 4.0, INF, INF, 12.8, INF, 7.0, INF, 9.9, 2.1, 6.3, 10.4, INF, 0.45, 1.3, 7.6, 4.6, 3.8, 11.9, 2.8, INF},						// PKL
//  {7.4, 13.7, 2.2, 7.5, INF, 7.3, INF, INF, 15.8, INF, 14.8, 6.8, 6.9, 6.6, 3.5, 3.7, 5.3, 6.9, 7.9, INF, 10.2, 12.4, 6.3, INF, 7.3, 4.7, INF, 5.3, 2.6, INF, INF, 14.2, INF, 1.4, 7.1, 0.3, INF, INF, 2.2, 6.7, 2.1, 5.5, 3.2, 1.9},								// PC
//  {INF, INF, INF, 7.8, INF, 10.4, INF, INF, INF, INF, INF, INF, 9.6, INF, 0.081, INF, INF, 7.5, 7.9, INF, 13.2, INF, 6.9, INF, INF, 5.3, INF, 6.9, INF, INF, INF, INF, 1.6, 4.9, INF, 3.6, INF, 0.022, 6.0, 3.0, INF, 6.1, INF, INF},								// PLY
//  {9.6, 7.6, 12.2, 11.9, 9.3, 18.5, INF, INF, 6.2, 10.4, 13.8, 16.5, 14.7, 8.0, 9.0, 11.4, 8.5, 7.8, 7.2, INF, 19.9, 20.0, 7.2, 8.3, 8.3, 7.8, INF, 12.8, 11.3, 13.3, INF, INF, 8.0, 12.8, 18.0, 13.1, INF, 9.3, 14.6, 9.5, 11.4, 11.7, 10.4, 11.2},				// PSP
//  {INF, 16.2, 4.8, 8.5, 1.7, 11.0, INF, INF, 13.3, 2.8, INF, 5.4, 10.2, 1.1, 2.3, 4.5, 1.8, 8.5, INF, INF, 13.9, 13.1, INF, 1.1, 1.9, 5.9, INF, 7.6, 4.4, 4.5, INF, 8.1, INF, 5.6, 11.1, 4.3, 3.0, 2.3, 6.2, 2.1, 4.4, 6.8, 3.9, 3.3},								// PPSM
//  {9.4, 20.9, 2.1, 8.7, 3.7, 7.8, INF, INF, 18.1, INF, 14.1, 6.5, 7.4, 5.8, 6.1, 3.4, 6.9, 10.2, 10.6, INF, 10.7, 9.0, 7.9, INF, 6.9, 6.3, INF, 7.8, 2.5, 1.0, 3.2, 12.4, 4.6, INF, 7.1, INF, 6.1, INF, 2.2, 9.3, 2.0, 7.1, 3.1, 1.8},								// QCMKL
//  {16.4, 29.5, 9.7, 13.1, 12.1, 10.7, INF, INF, 22.3, INF, 19.3, 8.4, 13.1, INF, 11.0, 7.7, 12.8, 14.6, 15.0, INF, 12.6, 2.2, 14.0, INF, 11.2, 12.4, INF, 12.2, 10.0, 7.0, INF, 18.1, 11.9, 6.9, INF, 14.3, 10.4, 10.9, 6.9, 12.4, 10.3, 13.3, 9.4, 8.4},			// SCM
//  {7.7, 13.6, 2.9, 6.2, 3.8, 7.3, INF, INF, 14.8, INF, 10.9, 7.2, 6.8, 6.5, 4.4, 6.3, 5.2, 6.8, 7.8, INF, 10.1, 10.1, 6.3, 4.3, 7.7, 4.6, INF, 5.3, 3.3, 0.4, INF, 10.7, 3.9, 1.5, 8.2, INF, INF, INF, 2.1, 7.6, 2.8, 5.5, INF, 2.6},								// SKL
//  {INF, INF, INF, INF, INF, INF, INF, INF, INF, 0.13, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF, INF},									// TS
//  {INF, INF, 2.2, 10.5, INF, 10.2, 0.3, INF, INF, 0.65, 12.7, INF, 9.2, 4.3, 0.65, 3.2, INF, 7.6, 8.1, INF, 16.1, 12.8, 7.0, INF, INF, 5.4, INF, 7.0, 0.95, 2.8, 0.022, INF, 1.7, 3.5, 10.9, INF, 0.85, INF, 8.1, 3.1, 1.9, 6.2, 3.3, 1.0},							// SWP
//  {9.5, 19.6, 3.9, 8.6, 5.6, 6.5, INF, INF, 16.5, INF, 12.9, 8.3, 6.1, 8.0, 6.2, 5.2, 7.0, 8.6, 9.0, INF, 11.6, 8.6, 8.0, INF, 8.5, 6.4, INF, 7.0, 4.3, 2.7, INF, 12.5, 5.7, 2.6, 6.7, 3.4, 7.7, INF, INF, 9.4, 3.8, 7.2, INF, 3.6},								// SPM
//  {0.65, 15.4, 6.2, 10.2, 3.1, 15.1, INF, INF, INF, 4.2, 17.1, 8.0, 11.9, INF, 3.7, 5.4, 2.8, 11.0, 10.4, INF, 17.3, 18.7, 10.4, 2.8, 0.65, 8.9, INF, 9.3, INF, 6.2, INF, 8.0, 2.2, 9.7, 12.0, 7.9, 4.0, 3.6, 9.2, INF, 5.4, INF, 4.4, INF},						// SVM
//  {INF, 18.1, 0.9, 9.3, INF, 9.3, INF, INF, 15.3, INF, 18.2, 3.8, 8.8, 4.5, INF, 1.8, INF, 10.0, 11.0, INF, 15.3, 11.0, 9.4, 5.6, 4.2, 12.8, INF, 12.5, 1.2, 2.5, INF, 13.3, 4.8, 3.1, 10.1, INF, INF, INF, 7.3, 5.3, INF, 13.6, 1.5, 1.3},							// SK
//  {11.0, 10.5, 13.9, 4.6, 10.9, 15.8, INF, INF, 12.5, INF, 9.4, 16.6, 10.5, 10.5, 8.3, 13.0, 10.1, 1.3, 2.7, INF, 17.1, 16.3, 0.75, 9.8, 13.7, 4.3, INF, 6.4, 12.9, 7.4, INF, 7.7, 10.4, 9.0, 14.4, 7.2, 11.5, INF, 9.7, 10.8, 13.1, INF, 12.1, 8.8},				// TGM
//  {5.1, 17.2, 1.9, 11.3, 4.1, 12.4, INF, INF, 14.3, INF, 19.2, 3.2, 10.1, 3.4, 3.0, 1.3, 6.5, 13.1, 12.6, INF, 14.3, 10.7, 12.6, INF, 3.2, 10.7, INF, 10.4, 2.0, 4.5, INF, 10.2, 3.9, 3.9, 9.9, INF, 2.4, 3.0, 5.1, 4.5, 1.3, 11.5, INF, 2.9},						// TLK
//  {INF, 17.8, 1.6, 7.6, 3.3, 9.0, INF, INF, INF, INF, 15.3, INF, 8.5, 4.2, 2.7, 2.5, INF, 7.3, 9.0, INF, 11.9, 13.1, INF, 3.7, 4.8, 5.1, INF, 6.7, 0.9, 2.2, 1.2, 10.7, 3.4, 2.8, 10.8, INF, INF, 1.3, 5.2, 5.0, 1.2, 5.9, 3.2, INF},								// TW
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
//// Randomize traffic condition {Congestion index: 0 <= x <= 10} [trafficMatrix]
//void randTrafficMatrix() {
//	for (int i = 0; i < N; i++) {
//		for (int j = 0; j < N; j++) {
//			if (distanceMatrix[i][j] != INF) {
//				// Generate random congestion index between 0 to 10
//				trafficMatrix[i][j].trfCondition = MIN_TRFCONDITION + rand() % (MAX_TRFCONDITION - MIN_TRFCONDITION + 1);
//			}
//			else {
//				// Set to INF if no direct path
//				trafficMatrix[i][j].trfCondition = INF;
//			}
//		}
//	}
//}
//
//// Initialize and randomize average speed matrix based on the traffic condition [avgSpeedMatrix]
//void initAverageSpeedMatrix() {
//	for (int i = 0; i < N; i++) {
//		for (int j = 0; j < N; j++) {
//			if (distanceMatrix[i][j] != INF) {
//				// Retrieve the current traffic condition
//				double trfCondition = trafficMatrix[i][j].trfCondition;
//
//				// Generate a random speed within the range of the traffic condition 
//				int maxSpeed = 0;
//				int minSpeed = 0;
//				if (trfCondition == 0.0) {  // Speed range: 80 <= x <= 150 (Acceptable speed)
//					maxSpeed = 150;
//					minSpeed = 80;
//				}
//				else if (trfCondition > 0.0 && trfCondition <= 5.0) { // Speed range: 51 <= x <= 79
//					maxSpeed = 79;
//					minSpeed = 51;
//				}
//				else if (trfCondition > 5.0 && trfCondition < 10.0) { // Speed range: 21 <= x <= 50
//					maxSpeed = 50;
//					minSpeed = 21;
//				}
//				else if (trfCondition == 10.0) { // Speed range: 8 <= x <= 20 (Congestion speed)
//					maxSpeed = 20;
//					minSpeed = 8;
//				}
//				avgSpeedMatrix[i][j].avgSpeed = minSpeed + (rand() % (maxSpeed - minSpeed + 1));
//
//				// Initialize the number of motorist and accumulated speed
//				avgSpeedMatrix[i][j].pathCount = 0;
//				avgSpeedMatrix[i][j].accSpeed = 0;
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
//// Initialize average travelling time matrix [avgTimeMatrix]
//void initAverageTrvTimeMatrix() {
//	for (int i = 0; i < N; i++) {
//		for (int j = 0; j < N; j++) {
//			if (distanceMatrix[i][j] != INF) {
//				// Average travelling time (in minutes) = (distance / average speed) x 60 
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
//// Calculate total distance of the route
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
//
//// ************************* Dijkstra's (Standard) Main Function *************************
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
//// *************** Function Declarations & Structures Related to MinHeap ***************
//// Heap node structure
//typedef struct HeapNode {
//	int vertex;         // Vertex number
//	float distance;     // Distance from source
//	int heapIndex;      // Index in the heap array
//} HeapNode;
//
//// Min heap structure
//typedef struct {
//	int size;			// Current number of heap nodes
//	int capacity;		// Total capacity of heap
//	int* pos;			// Keeps track of positions of heap nodes in pos[]
//	HeapNode** array;	// Array of pointers to heap nodes
//} MinHeap;
//
//// Function to create a new min heap node
//HeapNode* newHeapNode(int v, float dist) {
//	HeapNode* heapNode = (HeapNode*)malloc(sizeof(HeapNode)); // Allocate memory for heap node
//	heapNode->vertex = v;
//	heapNode->distance = dist;
//	return heapNode;
//}
//
//// Function to create a min heap
//MinHeap* createMinHeap(int capacity) {
//	MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
//	minHeap->pos = (int*)malloc(capacity * sizeof(int));
//	minHeap->size = 0; // Initialize size as 0 (heap is empty)
//	minHeap->capacity = capacity;
//	minHeap->array = (HeapNode**)malloc(capacity * sizeof(HeapNode*)); // Array is the actual heap
//	return minHeap;
//}
//
//// Function to free the MinHeap structure and its contents
//void freeMinHeap(MinHeap* heap) {
//	if (heap != NULL) {
//		if (heap->array != NULL) {
//			free(heap->array);
//		}
//		if (heap->pos != NULL) {
//			free(heap->pos);
//		}
//		free(heap);
//	}
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
//	if (isEmpty(minHeap)) {
//		printf("Heap is empty\n");
//		return NULL;
//	}
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
//// ******************** Dijkstra's (With Priority Queue) Main Function ********************
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
//// ******************* Function Declarations & Structures Related to A* *******************
//// A* node structure
//typedef struct {
//	int vertex;
//	float gCost;	// Cost from start to the current node
//	float hCost;	// Heuristic cost from the current node to the goal
//	float fCost;	// Total cost (gCost + hCost)
//	int prev;		// Predecessor node
//} AStarNode;
//
//// Heuristic function - Euclidean distance
//float heuristic(int src, int dst) {
//	float x1 = src % 11;
//	float y1 = src / 11;
//	float x2 = dst % 11;
//	float y2 = dst / 11;
//	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
//}
//
//// Update fCost in the minHeap
//void updateFcost(MinHeap* minHeap, int vertex, float gCost, float hCost) {
//	int i = minHeap->pos[vertex];
//	minHeap->array[i]->distance = gCost + hCost;
//
//	// Travel up while the complete tree is not heapified
//	while (i && minHeap->array[i]->distance < minHeap->array[(i - 1) / 2]->distance) {
//		// Swap this node with its parent
//		minHeap->pos[minHeap->array[i]->vertex] = (i - 1) / 2;
//		minHeap->pos[minHeap->array[(i - 1) / 2]->vertex] = i;
//		swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);
//
//		// Move to parent index
//		i = (i - 1) / 2;
//	}
//}
//
//// *********************************** A* Main Function ***********************************
//// A* Algorithm Function
//void aStar(int src, int dst, float dist[], int prev[], timeRoute_t graph[N][N]) {
//	MinHeap* minHeap = createMinHeap(N);
//
//	for (int v = 0; v < N; v++) {
//		dist[v] = INF;
//		prev[v] = -1;
//		minHeap->array[v] = newHeapNode(v, INF);
//		minHeap->pos[v] = v;
//	}
//
//	// Initialize source
//	dist[src] = 0;
//	updateFcost(minHeap, src, 0, heuristic(src, dst));
//
//	minHeap->size = N;
//
//	while (!isEmpty(minHeap)) {
//		HeapNode* minHeapNode = extractMin(minHeap);
//		int u = minHeapNode->vertex;
//
//		// Terminate when destination is reached
//		if (u == dst) break;
//
//		for (int v = 0; v < N; v++) {
//			if (graph[u][v].avgTrvTime != INF) {
//				float gCost = dist[u] + graph[u][v].avgTrvTime;
//				float hCost = heuristic(v, dst);
//				float fCost = gCost + hCost;
//
//				if (gCost < dist[v]) {
//					dist[v] = gCost;
//					prev[v] = u;
//					updateFcost(minHeap, v, gCost, hCost);
//				}
//			}
//		}
//	}
//
//	freeMinHeap(minHeap);
//}
//
//
//
//// User Query Generation Task [Sender Task]
//static void vUserQueryTask(void* pvParameters) {
//	srand(time(NULL));
//	query_t lValueToSend;
//	BaseType_t xStatus;
//
//	// Randomize user query's source and destination
//	for (unsigned int i = 0; i < MAX_QUERIES; i++) {
//
//		// Generate from randomized source to randomized destination
//		if (sel == 2 || sel == 5 || sel == 8) {
//			lValueToSend.srcNode = SRC_NODE;
//			do {
//				lValueToSend.dstNode = DST_NODE;
//			} while (lValueToSend.srcNode == lValueToSend.dstNode);
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
//		// Send data to the queue [queryQueue]
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
//					dijkstra(lReceivedValue.srcNode, dist, prev, avgTimeMatrix);
//				}
//				else if (sel == 4) {
//					dijkstraPQ(lReceivedValue.srcNode, dist, prev, avgTimeMatrix);
//				}
//				else if (sel == 5) {
//					dijkstraPQ(lReceivedValue.srcNode, dist, prev, avgTimeMatrix);
//				}
//				else if (sel == 6) {
//					dijkstraPQ(lReceivedValue.srcNode, dist, prev, avgTimeMatrix);
//				}
//				else if (sel == 7) {
//					aStar(lReceivedValue.srcNode, lReceivedValue.dstNode, dist, prev, avgTimeMatrix);
//				}
//				else if (sel == 8) {
//					aStar(lReceivedValue.srcNode, lReceivedValue.dstNode, dist, prev, avgTimeMatrix);
//				}
//				else if (sel == 9) {
//					aStar(lReceivedValue.srcNode, lReceivedValue.dstNode, dist, prev, avgTimeMatrix);
//				}
//				else {
//					vTaskEndScheduler();
//				}
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
//			if (sel == 1 || sel == 4 || sel == 7) {
//
//				// Calculate and print ETA
//				float totalETA = 0.0;
//				int currentVertex = lReceivedValue.dstNode;
//				while (currentVertex != lReceivedValue.srcNode && prev[currentVertex] != -1) {
//					totalETA += avgTimeMatrix[prev[currentVertex]][currentVertex].avgTrvTime;
//					currentVertex = prev[currentVertex];
//				}
//
//				// Release the token (mutex) 
//				xSemaphoreGive(xMutex);
//
//				// Check if a new batch of 1000 queries is completed or the end is reached
//				if (i % 1000 == 0 || i == MAX_QUERIES) {
//					// Calculate cumulative average time
//					float cumulativeAverageTime = (batchTotalTime / (float)i) * portTICK_PERIOD_MS;
//
//					if (i == 1000) {
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
//
//						vTaskEndScheduler();
//					}
//				}
//			}
//			else if (sel == 2 || sel == 5 || sel == 8) {
//				// Calculate and print ETA
//				float totalETA = 0.0;
//				int currentVertex = lReceivedValue.dstNode;
//				while (currentVertex != lReceivedValue.srcNode && prev[currentVertex] != -1) {
//					totalETA += avgTimeMatrix[prev[currentVertex]][currentVertex].avgTrvTime;
//					currentVertex = prev[currentVertex];
//				}
//
//				// Release the token (mutex) 
//				xSemaphoreGive(xMutex);
//
//				if (i % 1000 == 0) {
//					// Print the checking progress in percentage
//					printf("Checking Progress: %.2f%%\n", (float)i / MAX_QUERIES * 100);
//				}
//
//				if (i == MAX_QUERIES) {
//					vPrintString("\n");
//					vPrintString("                    Accuracy Checking                    \n");
//					vPrintString("---------------------------------------------------------\n");
//
//					// Print the source and destination
//					printf("Source\t\t\t: %s\n", mallNames[lReceivedValue.srcNode]);
//					printf("Destination\t\t: %s\n", mallNames[lReceivedValue.dstNode]);
//
//					vPrintString("---------------------------------------------------------\n");
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
//						vPrintString("---------------------------------------------------------\n");
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
//						vPrintString("---------------------------------------------------------\n");
//					}
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
//					vPrintString("---------------------------------------------------------\n");
//
//					vTaskEndScheduler();
//				}
//			}
//			else if (sel == 3 || sel == 6 || sel == 9) {
//				// Print generated query and user speed
//				printf("User %d\t: %s -> %s\n",
//					i,
//					mallNames[lReceivedValue.srcNode],
//					mallNames[lReceivedValue.dstNode]);
//
//				// Print shortest path
//				printf("Shortest Path\t: ");
//				printShortestPath(prev, lReceivedValue.srcNode, lReceivedValue.dstNode, mallNames);
//
//				// Print total distance
//				float totalDistance = totalDistCalculation(prev, lReceivedValue.srcNode, lReceivedValue.dstNode);
//				printf("\nDistance\t: %.2f km\n", totalDistance);
//
//				// Calculate and print ETA
//				float totalETA = 0.0;
//				int currentVertex = lReceivedValue.dstNode;
//				while (currentVertex != lReceivedValue.srcNode && prev[currentVertex] != -1) {
//					totalETA += avgTimeMatrix[prev[currentVertex]][currentVertex].avgTrvTime;
//					currentVertex = prev[currentVertex];
//				}
//				printf("ETA\t\t: %.0f minute[s]\n\n", (fmod(totalETA, 1) < 0.4) ? floor(totalETA) : ceil(totalETA));
//
//				// Release the token (mutex) 
//				xSemaphoreGive(xMutex);
//
//				if (i == MAX_QUERIES) {
//					xTaskNotifyGive(xMotoristDataTaskHandle);
//					xTaskNotifyGive(xUpdateMatricesTaskHandle);
//					break;
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
//	vTaskDelete(NULL);
//}
//
//// Motorist Path & Speed Generation Task [Sender Task]
//static void vMotoristDataTask(void* pvParameters) {
//	motorist_batch_t motoristBatch;
//	unsigned int batchCount = 0;
//	BaseType_t xStatus;
//
//	// Randomize path and speed
//	while (true) {
//		motorist_t lValueToSend;
//
//		// Generate random single path
//		lValueToSend.srcNode = rand() % N;
//		do {
//			lValueToSend.dstNode = rand() % N;
//		} while (lValueToSend.srcNode == lValueToSend.dstNode || distanceMatrix[lValueToSend.srcNode][lValueToSend.dstNode] == INF);
//
//		// Randomize traffic condition for every new batch of motorists
//		randTrafficMatrix();
//
//		// Retrieve the traffic condition
//		double trfCondition = trafficMatrix[lValueToSend.srcNode][lValueToSend.dstNode].trfCondition;
//
//		// Generate speed randomly based on traffic condition with the piecewise function
//		int maxSpeed = 0;
//		int minSpeed = 0;
//		if (trfCondition == 0.0) { // Speed range: 80 <= x <= 150 (Acceptable speed)
//			maxSpeed = 150;
//			minSpeed = 80;
//		}
//		else if (trfCondition > 0.0 && trfCondition <= 5.0) { // Speed range: 51 <= x <= 79
//			maxSpeed = 79;
//			minSpeed = 51;
//		}
//		else if (trfCondition > 5.0 && trfCondition < 10.0) { // Speed range: 21 <= x <= 50
//			maxSpeed = 50;
//			minSpeed = 21;
//		}
//		else if (trfCondition == 10.0) { // Speed range: 8 <= x <= 20 (Congestion speed)
//			maxSpeed = 20;
//			minSpeed = 8;
//		}
//		lValueToSend.mtSpeed = minSpeed + (rand() % (maxSpeed - minSpeed + 1));
//
//		// Add the generated motorist data to a single batch
//		motoristBatch.motorists[batchCount++] = lValueToSend;
//
//		// Check if the batch count is full
//		if (batchCount >= MOTORIST_BATCH_SIZE) {
//			// Send the batch to the queue [mtSpeedQueue]
//			xStatus = xQueueSendToBack(mtSpeedQueue, &motoristBatch, portMAX_DELAY);
//
//			if (xStatus != pdPASS) {
//				vPrintString("Could not send to the queue. \r\n");
//			}
//
//			// Reset the batch count for the next batch
//			batchCount = 0;
//
//			// Check for a notification to terminate after generated all user query
//			// It returns 0 if no notification, but if there is a notification found, it returns
//			// other number, which subsequently break the task.
//			if (ulTaskNotifyTake(pdFALSE, 0) != 0) {
//				vTaskEndScheduler();
//			}
//		}
//	}
//	vTaskDelete(NULL);
//}
//
//// THESE FUNCTIONS ARE MAINLY FOR THE TASK [vUpdateMatricesTask]
//// Update average speed matrix
//void uptAvgSpeedMatrix(int srcNode, int dstNode, double mtSpeed) {
//
//	if (distanceMatrix[srcNode][dstNode] != INF) {
//		avgSpeedMatrix[srcNode][dstNode].pathCount += 1; // Accumulate number of motorist
//		avgSpeedMatrix[srcNode][dstNode].accSpeed += mtSpeed; // Accumulate motorists' speeds
//		avgSpeedMatrix[srcNode][dstNode].avgSpeed = avgSpeedMatrix[srcNode][dstNode].accSpeed / avgSpeedMatrix[srcNode][dstNode].pathCount; // Calculate average speed (Accumulated Speed / Number of Motorist)
//	}
//}
//
//// Update average travelling time
//void uptAvgTrvTimeMatrix(int srcNode, int dstNode) {
//	if (distanceMatrix[srcNode][dstNode]) {
//		// Average traveling time (in minutes) = (distance / average speed) * 60
//		avgTimeMatrix[srcNode][dstNode].avgTrvTime = (distanceMatrix[srcNode][dstNode] / avgSpeedMatrix[srcNode][dstNode].avgSpeed) * 60;
//	}
//}
//
//// Update Matrices Task [Receiver Task]
//static void vUpdateMatricesTask(void* pvParameters) {
//	motorist_batch_t lReceivedValue;
//	BaseType_t xStatus;
//	char* printUpdate = (char*)pvParameters;
//
//	// Declare xLastWakeTime as tick type
//	TickType_t xLastWakeTime;
//
//	// Record the tick count at the start of the task's execution
//	xLastWakeTime = xTaskGetTickCount();
//
//	while (true) {
//		// Set 500 ticks for this task's periodic execution
//		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(500));
//
//		// Receive data from the queue [mtSpeedQueue]
//		xStatus = xQueueReceive(mtSpeedQueue, &lReceivedValue, portMAX_DELAY);
//
//		if (xStatus == pdPASS) {
//
//			// Take the token (mutex) before proceed the operations
//			if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
//
//				// Reset pathCount and accSpeed to 0
//				for (int i = 0; i < N; i++) {
//					for (int j = 0; j < N; j++) {
//						if (distanceMatrix[i][j] != INF) {
//							avgSpeedMatrix[i][j].pathCount = 0;
//							avgSpeedMatrix[i][j].accSpeed = 0;
//						}
//					}
//				}
//
//				// Process each motorist from the received batch
//				for (unsigned int i = 0; i < MOTORIST_BATCH_SIZE; i++) {
//					motorist_t motorist = lReceivedValue.motorists[i];
//					int src = motorist.srcNode;
//					int dst = motorist.dstNode;
//					double mtSpeed = motorist.mtSpeed;
//
//					// Update average speed and average travelling time matrices 
//					uptAvgSpeedMatrix(src, dst, mtSpeed);
//					uptAvgTrvTimeMatrix(src, dst);
//				}
//				// Release the token (mutex) after updating the matrices
//				xSemaphoreGive(xMutex);
//
//				// Check for a notification to terminate after generated all user query
//				// It returns 0 if no notification, but if there is a notification found, it returns
//				// other number, which subsequently break the task.
//				if (ulTaskNotifyTake(pdFALSE, 0) != 0) {
//					vTaskEndScheduler();;
//				}
//			}
//			vPrintString(printUpdate);
//		}
//		else {
//			vPrintString("Could not receive from the queue. \r\n");
//		}
//	}
//	vTaskDelete(NULL);
//}
//
//int main(void) {
//	srand(time(NULL)); // Initialize random seed
//
//	randTrafficMatrix(); // Initialize the random traffic condition
//	initAverageSpeedMatrix(); // Initialize the average speed
//	initAverageTrvTimeMatrix(); // Initialize the average travelling time
//
//	// Create queue
//	queryQueue = xQueueCreate(1, sizeof(query_t));
//	mtSpeedQueue = xQueueCreate(1, sizeof(motorist_batch_t));
//
//	// Create semaphore
//	xMutex = xSemaphoreCreateMutex();
//
//	vPrintString("Main Menu:\n");
//	vPrintString("1. Dijkstra's Algorithm (Standard) - Evaluate Performance\n");
//	vPrintString("2. Dijkstra's Algorithm (Standard) - Check Accuracy\n");
//	vPrintString("3. Dijkstra's Algorithm (Standard) - User Query Automation Generation\n");
//	vPrintString("4. Dijkstra's Algorithm (With Priority Queue) - Evaluate Performance\n");
//	vPrintString("5. Dijkstra's Algorithm (With Priority Queue) - Check Accuracy\n");
//	vPrintString("6. Dijkstra's Algorithm (With Priority Queue) - User Query Automation Generation\n");
//	vPrintString("7. A* Algorithm - Evaluate Performance\n");
//	vPrintString("8. A* Algorithm - Check Accuracy\n");
//	vPrintString("9. A* Algorithm - User Query Automation Generation\n");
//	vPrintString("10. Exit\n");
//	printf("\nEnter your choice: ");
//	scanf("%d", &sel);
//	printf("\n");
//
//	if (sel == 10) {
//		vPrintString("Program exiting...\n");
//		return 0;
//	}
//
//	// Create tasks
//	if (queryQueue != NULL && xMutex != NULL) {
//		xTaskCreate(vUserQueryTask,	// Task function
//			"User Query Task",		// Task name
//			1024,					// Stack size
//			NULL,					// Task parameter
//			1,						// Task priority
//			NULL);					// Task handle
//
//		xTaskCreate(vPathCalculationTask,
//			"Path Calculation Task",
//			1024,
//			NULL,
//			2,
//			NULL);
//
//		xTaskCreate(vMotoristDataTask,
//			"Motorist Data Generation Task",
//			1024,
//			NULL,
//			1,
//			&xMotoristDataTaskHandle);
//
//		xTaskCreate(vUpdateMatricesTask,
//			"Matrices Update Task",
//			1024,
//			"======================== UPDATE TRAFFIC DATA ========================\n\n",
//			3,
//			&xUpdateMatricesTaskHandle);
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