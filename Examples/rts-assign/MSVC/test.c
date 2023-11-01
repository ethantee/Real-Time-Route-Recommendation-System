#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#define N 11
#define INF FLT_MAX // FLT_MAX is the maximum value of a float

int totalQueriesGlobal = 0;

// Enumerate shopping malls in adjacency matrix
enum Malls {
    AMTM, APBJ, AKSM, BSC, BTS, BMSC, BBP, DCM, EP, F88, GDSM
};

// Define a struct for a node in the graph
typedef struct Node {
    int id;
    int parent_id;
    float g_cost;
    float h_cost;
    float f_cost;
} Node;

// Distance adjacency matrix between shopping malls
float distanceMatrix[11][11] = {
    {INF, 14.5, 7.6, 13, 4, 15.2, INF, INF, 14.8, 5.4, 17.7},
    {14.6, INF, 17.4, 13.6, 14.4, 26.1, INF, INF, 5.3, INF, 14.5},
    {7.3, 22.1, INF, 9.6, INF, 10.9, INF, INF, 15.2, INF, 16.4},
    {11.2, 13.1, INF, INF, INF, 9.9, INF, 1.7, 16.4, INF, 5.5},
    {3.4, INF, INF, 9.1, INF, 13, INF, INF, 12.5, INF, 15.9},
    {17.4, 22.8, INF, 11.1, INF, INF, INF, INF, 22.7, INF, 12.2},
    {INF, INF, INF, 8, INF, 10.5, INF, INF, 12.6, INF, 12.7},
    {11.5, 16.9, INF, INF, 8.5, 9.4, 9.3, INF, 18.4, INF, 6.2},
    {11.5, 4, 14.9, 14.8, 11.9, 20.7, INF, INF, INF, INF, 16.7},
    {INF, INF, INF, INF, 0.85, INF, INF, INF, 13.3, INF, INF},
    {16.2, 17.3, 14, 6.6, INF, 13.1, INF, INF, 18.5, INF, INF}
};

// Traffic adjacency matrix between shopping malls
float trafficMatrix[11][11];

// Heuristic function
float heuristic(int source, int target) {
    // For simplicity, use 0 as heuristic (turns A* into Dijkstra's algorithm)
    return 0;
}

// Task to generate user queries
void generateQuery(void* pvParameters) {
    int totalQueries = 0;
    while (totalQueries < 15) {
        int source = rand() % 11;
        int target = rand() % 11;
        
        // Ensure source and target are not the same
        while (source == target) {
            target = rand() % 11;
        }

        // Print User query number and source-destination pair
        printf("User %d: %d -> %d\n", totalQueries + 1, source, target);

        Node nodes[N];
        int visited[N] = { 0 };

        for (int i = 0; i < N; i++) {
            nodes[i].id = i;
            nodes[i].parent_id = -1;
            nodes[i].g_cost = INF;
            nodes[i].h_cost = INF;
            nodes[i].f_cost = INF;
        }

        nodes[source].g_cost = 0;
        nodes[source].h_cost = heuristic(source, target);
        nodes[source].f_cost = nodes[source].g_cost + nodes[source].h_cost;

        int found = 0;
        while (!found) {
            // Find the node with the lowest f_cost
            Node* current = NULL;
            float min_f_cost = INF;
            for (int i = 0; i < N; i++) {
                if (!visited[i] && nodes[i].f_cost < min_f_cost) {
                    min_f_cost = nodes[i].f_cost;
                    current = &nodes[i];
                }
            }

            if (current == NULL) {
                printf("No direct path found.\n\n");
                break;
            }

            visited[current->id] = 1;  // Mark as visited

            if (current->id == target) {
                // Reconstruct the path
                int path[N];
                int pathLength = 0;

                int current_id = target;
                while (current_id != source) {
                    path[pathLength++] = current_id;
                    current_id = nodes[current_id].parent_id;
                }
                path[pathLength++] = source;

                // Display path in reverse order
                printf("Shortest path: ");
                for (int i = pathLength - 1; i >= 0; i--) {
                    printf("%d", path[i]);
                    if (i > 0) {
                        printf(" -> ");
                    }
                }
                printf("\nDistance: %.2f KM\n", nodes[target].g_cost);
                printf("\n");
                found = 1;
                break;
            }

            // Update neighbours
            for (int i = 0; i < N; i++) {
                if (distanceMatrix[current->id][i] == INF || visited[i]) continue;
                float tentative_g_cost = current->g_cost + distanceMatrix[current->id][i];
                if (tentative_g_cost < nodes[i].g_cost) {
                    nodes[i].parent_id = current->id;
                    nodes[i].g_cost = tentative_g_cost;
                    nodes[i].h_cost = heuristic(i, target);
                    nodes[i].f_cost = nodes[i].g_cost + nodes[i].h_cost;
                }
            }
        }

        // Increment the traffic matrix
        trafficMatrix[source][target] += 1;

        vTaskDelay(pdMS_TO_TICKS(100));
        totalQueries++;
        totalQueriesGlobal = totalQueries;
    }

    vTaskDelete(NULL);
}

void monitorQueries(void* pvParameters) {
    while (totalQueriesGlobal < 15) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // Print the traffic matrix
    printf("Traffic Condition Matrix:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%.1f ", trafficMatrix[i][j]);
        }
        printf("\n");
    }

    exit(0);
}

int main() {
    // Initialise the traffic matrix to 1
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            trafficMatrix[i][j] = 1;
        }
    }

    xTaskCreate(generateQuery, "QueryGenerator", 1000, NULL, 1, NULL);
    xTaskCreate(monitorQueries, "QueryMonitor", 1000, NULL, 1, NULL);

    vTaskStartScheduler();

    return 0;
}