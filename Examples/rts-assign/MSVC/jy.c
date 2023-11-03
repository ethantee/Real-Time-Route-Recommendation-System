#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>

#include "supporting_functions.h"

#define N 5

// Enumerate shopping malls in adjacency matrix
enum Malls {
    AMTM, APBJ, AKSM, BSC, BTS
};

// Array of mall names corresponding to their enumeration
const char *mallNames[] = {"AMTM", "APBJ", "AKSM", "BSC", "BTS"};

// Connection adjacency matrix between shopping malls; value = {-1, 1} - static
int connectionMatrix[N][N] = {
  {-1, 1, 1, 1, 1},    // Connections from AEON Mall Taman Maluri (A_M_T_M)
  {1, -1, 1, 1, 1},    // Connections from Aurora Place Bukit Jalil (A_P_B_J)
  {1, 1, -1, 1, -1},    // Connections from Avenue K Shopping Mall (A_K_S_M)
  {1, 1, -1, -1, -1},    // Connections from Bangsar Shopping Centre (B_S_C)
  {1, -1, -1, 1, -1},    // Connections from Berjaya Times Square (B_T_S)
};

// Distance adjacency matrix between shopping malls; value = {distances, -1} - static
double distanceMatrix[N][N] = {
  {-1.0, 14.5, 7.6, 13.0, 4.0},    // Distances from AEON Mall Taman Maluri (A_M_T_M)
  {14.6, -1.0, 17.4, 13.6, 14.4},    // Distances from Aurora Place Bukit Jalil (A_P_B_J)
  {7.3, 22.1, -1.0, 9.6, -1.0},    // Distances from Avenue K Shopping Mall (A_K_S_M)
  {11.2, 13.1, -1.0, -1.0, -1.0},    // Distances from Bangsar Shopping Centre (B_S_C)
  {3.4, -1.0, -1.0, 9.1, -1.0},    // Distances from Berjaya Times Square (B_T_S)
};

// Traffic condition adjacency matrix between shopping malls; value = {1 < x < 5, -1} - dynamic
double trafficMatrix[N][N];

// Edges weight adjacency matix between shopping malls; - dynamic
// Weight values = distance * traffic condition
double weightMatrix[N][N];

// Function to initialize traffic condition adjacency matrix with 1s and -1s
void initializeTrafficConditions() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (connectionMatrix[i][j] == 1) {
                trafficMatrix[i][j] = connectionMatrix[i][j];  // Initial value
            }
            else {
                trafficMatrix[i][j] = -1.0;  // No connection
            }
        }
    }
}

// Function to initialize weight condition adjacency matrix with distance values and -1s
void initializeWeightConditions() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (connectionMatrix[i][j] == 1) {
                weightMatrix[i][j] = distanceMatrix[i][j];  // Initial value
            }
            else {
                weightMatrix[i][j] = -1.0;  // No connection
            }
        }
    }
}

// Function to update Traffic Condition Matrix when a user queries a path
void updateTrafficMatrix(int source, int destination) {
    if (connectionMatrix[source][destination] == 1) {
        // Check if traffic value has reached the maximum value of 5
        if (trafficMatrix[source][destination] < 5.0) {
            // Increment the traffic condition value by 1
            trafficMatrix[source][destination] += 1.0;
        } 
        // If traffic value is already at 5, do nothing (i.e., it stays at 5)
    }
    else {
        // Handle the case where there's no direct path between source and destination
        printf("No direct path between the selected malls.\n");
    }
}

// Function to update weightMatrix based on trafficMatrix and distanceMatrix
void updateWeightMatrix() {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (trafficMatrix[i][j] != -1.0 && distanceMatrix[i][j] != -1.0) {
                weightMatrix[i][j] = trafficMatrix[i][j] * distanceMatrix[i][j];
            }
            else {
                weightMatrix[i][j] = -1.0;  // No connection or invalid distance
            }
        }
    }
}

int main() {
    // Seed the random number generator
    srand(time(0));

    // Initialize the traffic conditions - TESTING
    initializeTrafficConditions();

    // Simulate 5 users querying for routes
    for (int i = 0; i < 5; ++i) {
        int source = rand() % N;
        int destination;
        do {
            destination = rand() % N;
        } while (source == destination); // Ensure source and destination are not the same

        // Generate a random speed for the user in km/h, e.g., between 20 and 100
        int speed = rand() % 81 + 20;  // This will generate a random number between 20 and 100

        printf("User %d is querying from %s to %s at a speed of %d km/h\n", i + 1, mallNames[source], mallNames[destination], speed);
        updateTrafficMatrix(source, destination); // Increment traffic condition
    }

    // Print Adjacency Matrix - TESTING 
    printf("\n Traffic condition matrix \n");
    for(int i = 0; i < N; ++i) {
        // Loop through columns
        for (int j = 0; j < N; ++j) {
            printf("%6.2f ", trafficMatrix[i][j]);  // Print each element with a space
        }
        printf("\n");  // Move to the next line after each row
    }

    // Initialize and update the weight matrix - TESTING
    initializeWeightConditions();
    updateWeightMatrix();

    // Print Weight Matrix - TESTING
    printf("\n Weight matrix \n");
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            printf("%6.2f ", weightMatrix[i][j]);  // Print each element with a space
        }
        printf("\n");  // Move to the next line after each row
    }

    return 0;
}