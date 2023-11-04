#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stdbool.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "supporting_functions.h"

#define N 11 // Number of malls
#define INF FLT_MAX // Use this for infinity in distances
#define MIN_SPEED 20 // Minimum speed in KM/H
#define MAX_SPEED 100 // Maximum speed in KM/H

// Enumerate shopping malls
enum Malls {
    AMTM, APBJ, AKSM, BSC, BTS, BMSC, BBP, DCM, EP, F88, GDSM
};

// Mall names
const char* mallNames[N] = { "AMTM", "APBJ", "AKSM", "BSC", "BTS", "BMSC", "BBP", "DCM", "EP", "F88", "GDSM" };

// Connection adjacency matrix (1 = connected, -1 = not connected) - static
int connectionMatrix[11][11] = {
    {INF, 1, 1, 1, 1, 1, INF, INF, 1, 1, 1},            // AMTM
    {1, INF, 1, 1, 1, 1, INF, INF, 1, INF, 1},          // APBJ
    {1, 1, INF, 1, INF, 1, INF, INF, 1, INF, 1},        // AKSM
    {1, 1, INF, INF, INF, 1, INF, 1, 1, INF, 1},        // BSC
    {1, INF, INF, 1, INF, 1, INF, INF, 1, INF, 1},      // BTS
    {1, 1, INF, 1, INF, INF, INF, INF, 1, INF, 1},      // BMSC
    {INF, INF, INF, 1, INF, 1, INF, INF, 1, INF, 1},    // BBP
    {1, 1, INF, INF, 1, 1, 1, INF, 1, INF, 1},          // DCM
    {1, 1, 1, 1, 1, 1, INF, INF, INF, INF, 1},          // EP
    {1, INF, INF, INF, 1, INF, INF, INF, 1, INF, INF},  // F88
    {1, 1, 1, 1, INF, 1, INF, INF, 1, INF, INF}         // GDSM
};

// Distance adjacency matrix (in KM) - static
float distanceMatrix[11][11] = {
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

// Traffic condition adjacency matrix (1 = light traffic, 5 = heavy traffic) - dynamic
double trafficMatrix[N][N];

// Weight adjacency matrix (Distance * Traffic) - dynamic
double weightMatrix[N][N];

void incrementTrafficOnPath(int path[], int pathLength) {
    for (int i = 0; i < pathLength - 1; i++) {
        int from = path[i];
        int to = path[i + 1];

        if (trafficMatrix[from][to] != -1) {
            trafficMatrix[from][to] += 0.5;
        }
    }
}

// Function to generate a random speed for each user query
int generateRandomSpeed() {
    return MIN_SPEED + (rand() % (MAX_SPEED - MIN_SPEED + 1));
}

// Update the weight matrix based on the current traffic conditions
void updateWeightMatrix() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (trafficMatrix[i][j] != -1) {
                weightMatrix[i][j] = distanceMatrix[i][j] * trafficMatrix[i][j];
            }
            else {
                weightMatrix[i][j] = INF;
            }
        }
    }
}

// Function to print the traffic matrix
void printTrafficMatrix() {
    printf("Traffic Matrix:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (trafficMatrix[i][j] == -1) {
                printf("INF\t");
            }
            else {
                printf("%.1f\t", trafficMatrix[i][j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}

// Define a struct for a node
typedef struct Node {
    int id;
    int parent_id;
    float g_cost;
    float h_cost;
    float f_cost;
} Node;

// Heuristic function
float heuristic(int source, int target) {
    return 0; // Zero heuristic turns A* into Dijkstra's algorithm
}

// A* algorithm function to find the shortest path
void findShortestPath(int source, int target, Node nodes[N], int path[N], int* pathLength) {
    int visited[N] = { 0 };

    // Initialize nodes
    for (int i = 0; i < N; i++) {
        nodes[i].id = i;
        nodes[i].parent_id = -1;
        nodes[i].g_cost = INF;
        nodes[i].h_cost = heuristic(i, target); // Precompute heuristic for all nodes
        nodes[i].f_cost = INF;
    }

    nodes[source].g_cost = 0;
    nodes[source].f_cost = nodes[source].h_cost; // For the source, f_cost is simply the heuristic

    while (true) {
        Node* current = NULL;
        float min_f_cost = INF;

        // Find the node with the lowest f_cost
        for (int i = 0; i < N; i++) {
            if (!visited[i] && nodes[i].f_cost < min_f_cost) {
                min_f_cost = nodes[i].f_cost;
                current = &nodes[i];
            }
        }

        // No path exists or target found
        if (current == NULL || current->id == target) {
            break;
        }

        visited[current->id] = 1; // Mark as visited

        // Update neighbours based on the weightMatrix instead of distanceMatrix
        for (int i = 0; i < N; i++) {
            if (weightMatrix[current->id][i] != INF && !visited[i]) { // Use weightMatrix
                float tentative_g_cost = current->g_cost + weightMatrix[current->id][i];
                if (tentative_g_cost < nodes[i].g_cost) {
                    nodes[i].parent_id = current->id;
                    nodes[i].g_cost = tentative_g_cost;
                    nodes[i].f_cost = nodes[i].g_cost + nodes[i].h_cost; // Update f_cost
                }
            }
        }
    }

    // Reconstruct the path
    *pathLength = 0;
    int current_id = target;
    while (current_id != source && current_id != -1) {
        path[(*pathLength)++] = current_id;
        current_id = nodes[current_id].parent_id;
    }
    if (current_id != -1) {
        path[(*pathLength)++] = source; // Add the source at the end
    }
    // Reverse the path array to get the correct order from source to target
    for (int i = 0; i < *pathLength / 2; i++) {
        int temp = path[i];
        path[i] = path[*pathLength - i - 1];
        path[*pathLength - i - 1] = temp;
    }
}

// Task to generate user queries
void generateQuery(void* pvParameters) {
    // Seed the random number generator
    srand(time(NULL));

    int totalQueries = 0;
    while (totalQueries < 100) {
        printTrafficMatrix();

        int source = rand() % N;
        int target = rand() % N;

        // Ensure source and target are not the same
        while (source == target) {
            target = rand() % N;
        }

        int speed = generateRandomSpeed(); // Generate a random speed for this user query

        // Print User query number and source-destination pair
        printf("User %d\t\t: %d -> %d\n", totalQueries + 1, source, target);

        Node nodes[N]; // Create an array of nodes for the A* algorithm
        int path[N]; // Array to store the shortest path
        int pathLength = 0; // Variable to store the length of the path

        findShortestPath(source, target, nodes, path, &pathLength);

        if (pathLength > 0) { // If a path is found
            incrementTrafficOnPath(path, pathLength); // Increment the traffic along the path
            updateWeightMatrix(); // Update the weight matrix based on the new traffic conditions
        }

        // Calculate travelling time in hours and then convert to minutes
        float travellingTimeHours = nodes[target].g_cost / speed;
        float travellingTimeMinutes = travellingTimeHours * 60;

        // Print the shortest path, speed, distance and travelling time
        printf("Shortest Path\t: ");
        for (int i = 0; i < pathLength; i++) {
            printf("%d", path[i]);
            if (i < pathLength - 1) {
                printf(" -> ");
            }
        }

        printf("\nSpeed\t\t: %d km/h\n", speed);
        printf("Distance\t: %.2f km\n", nodes[target].g_cost);
        printf("Travelling Time\t: %.2f minute(s)\n\n", travellingTimeMinutes);

        // Increment totalQueries after processing each query
        totalQueries++;

        vTaskDelay(5); // Delay for task scheduling
    }

    vTaskDelete(NULL); // Delete the task when done
}

int main() {
    // Set initial traffic conditions to 1.0 for all connected paths
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (distanceMatrix[i][j] != INF) {
                trafficMatrix[i][j] = 1.0; // Initial traffic condition
            }
            else {
                trafficMatrix[i][j] = -1; // No connection
            }
        }
    }

    updateWeightMatrix();
    xTaskCreate(generateQuery, "QueryGenerator", 1000, NULL, 1, NULL);
    vTaskStartScheduler();

    return 0;
}