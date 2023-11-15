#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stdbool.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "supporting_functions.h"

#define N 44 // Number of malls

typedef struct {
    int src;
    int dest;
} query_t;

QueueHandle_t xQueue;
query_t user_query[1000];

// Enumerate shopping malls
enum Malls {
    AMTM, APBJ, AKSM, BSC, BTS, BMSC, BBP, DCM, EP, F88, GDSM,
    GEM, HSC, IC, IPKL, IM, KFW, IKEC, KGM, L10, LK, MSM,
    MVM, LBBCC, MSC, NSSM, PBJ, PDH, PKL, PC, PLY, PSP, PPSM,
    QCMKL, SCM, SKL, TS, SWP, SPM, SVM, SK, TGM, TLK, TW
};

// Distance adjacency matrix (in KM, INF = not connected) - static
double distanceMatrix[44][44] = {
  {-1.0, 14.5, 7.6, 13.0, 4.0, 15.2, -1.0, -1.0, 14.8, 5.4, 17.7, 6.5, 14.7, -1.0, -1.0, 6.7, 3.6, 11.5, 11.7, -1.0, 20.3, 17.2, 11.0, 3.7, 1.6, 10.3, -1.0, 12.0, -1.0, -1.0, -1.0, 9.9, 3.2, -1.0, 14.3, 8.7, 5.6, 4.1, 10.6, 1.2, 6.8, 10.2, 5.8, -1.0},    // Distances from AEON Mall Taman Maluri (A_M_T_M)
  {14.6, -1.0, 17.4, 13.6, 14.4, 26.1, -1.0, -1.0, 5.3, -1.0, 14.5, 17.5, 19.0, -1.0, 13.9, 16.5, 16.8, 10.5, 11.1, -1.0, 23.3, 24.6, 10.0, -1.0, 13.6, 10.6, 1.0, 16.3, 16.4, 14.4, -1.0, 10.0, 14.1, 20.3, 23.2, 14.2, 15.2, -1.0, 19.5, 13.0, 16.6, -1.0, 15.4, -1.0},    // Distances from Aurora Place Bukit Jalil (A_P_B_J)
  {7.3, 22.1, -1.0, 9.6, -1.0, 10.9, -1.0, -1.0, 15.2, -1.0, 16.4, 3.4, 10.5, -1.0, -1.0, 1.3, 4.7, 13.2, 13.5, -1.0, 13.5, 10.2, 12.1, -1.0, -1.0, 10.5, -1.0, 9.5, 2.3, 2.8, -1.0, -1.0, -1.0, 2.7, 8.3, -1.0, -1.0, -1.0, 5.4, -1.0, 1.0, 8.9, 1.0, 2.2},    // Distances from Avenue K Shopping Mall (A_K_S_M)
  {11.2, 13.1, -1.0, -1.0, -1.0, 9.9, -1.0, 1.7, 16.4, -1.0, 5.5, 14.9, 5.7, -1.0, -1.0, 11.3, 9.8, 4.7, 4.3, -1.0, 11.5, 15.0, 4.2, -1.0, -1.0, 4.3, -1.0, 2.0, 8.3, 7.2, -1.0, 11.7, 9.3, 8.8, 13.1, 7.0, -1.0, -1.0, 8.5, 11.0, 10.3, 3.4, 11.7, 7.4},    // Distances from Bangsar Shopping Centre (B_S_C)
  {3.4, -1.0, -1.0, 9.1, -1.0, 13.0, -1.0, -1.0, 12.5, -1.0, 15.9, 7.1, 10.8, -1.0, -1.0, 6.0, 1.2, 7.8, 7.9, -1.0, 15.2, 14.9, 7.2, -1.0, 4.0, 5.4, -1.0, 8.2, -1.0, -1.0, -1.0, 8.4, 1.3, -1.0, 14.1, -1.0, -1.0, -1.0, -1.0, 2.7, -1.0, 7.6, -1.0, 3.9},    // Distances from Berjaya Times Square (B_T_S)
  {17.4, 22.8, -1.0, 11.1, -1.0, -1.0, -1.0, -1.0, 22.7, -1.0, 12.2, 13.8, 7.2, -1.0, 12.4, 10.8, 15.7, 13.4, 14.8, -1.0, 5.6, 10.6, 12.8, -1.0, 15.3, 12.2, -1.0, 10.2, 10.5, 8.5, -1.0, 18.7, 14.4, 7.6, 10.5, 9.7, -1.0, -1.0, 6.9, 15.3, -1.0, 12.1, -1.0, 9.8},    // Distances from Brem Mall Shopping Complex (B_M_S_C)
  {-1.0, -1.0, -1.0, 8.0, -1.0, 10.5, -1.0, -1.0, 12.6, -1.0, 12.7, 6.7, 9.7, -1.0, 0.3, 3.7, -1.0, 7.6, 8.0, -1.0, 13.7, 13.4, 7.1, -1.0, 2.7, 5.4, -1.0, 7.1, -1.0, 3.7, -1.0, 8.6, 1.7, 4.2, 11.4, 3.6, -1.0, -1.0, 6.0, 3.2, -1.0, 6.3, -1.0, 1.5},    // Distances from Bukit Bintang Plaza (B_B_P)
  {11.5, 16.9, -1.0, -1.0, 8.5, 9.4, 9.3, -1.0, 18.4, -1.0, 6.2, -1.0, 6.3, -1.0, -1.0, 10.5, 8.8, 6.8, 5.6, -1.0, 10.7, 13.8, 6.2, -1.0, -1.0, 6.4, -1.0, 0.1, 7.6, 6.0, -1.0, 13.3, 8.5, 7.6, 11.9, 5.9, 8.0, -1.0, 7.3, 11.4, -1.0, 4.9, -1.0, 6.8},    // Distances from Damansara City Mall (D_C_M)
  {11.5, 4.0, 14.9, 14.8, 11.9, 20.7, -1.0, -1.0, -1.0, -1.0, 16.7, 15.7, 17.6, -1.0, -1.0, 14.3, 11.2, 10.7, 12.2, -1.0, 22.8, 26.4, 10.1, 11.2, 11.2, 11.2, -1.0, 17.2, 13.6, 14.1, -1.0, 4.6, 10.9, 18.0, 20.9, 14.0, -1.0, 11.8, 18.1, 10.7, 14.3, 9.4, 13.3, -1.0},    // Distances from Endah Parade (E_P)
  {-1.0, -1.0, -1.0, -1.0, 0.85, -1.0, -1.0, -1.0, 13.3, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 12.4, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 1.6, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 3.7, -1.0, -1.0, -1.0, -1.0},    // Distances from Fahrenheit88 (F_88)
  {16.2, 17.3, 14.0, 6.6, -1.0, 13.1, -1.0, -1.0, 18.5, -1.0, -1.0, 17.8, 7.6, 15.3, -1.0, 15.2, 14.5, 8.5, 7.6, -1.0, 10.8, 19.7, 8.2, -1.0, 15.3, 10.5, 15.5, 6.0, -1.0, 11.1, -1.0, 13.8, 13.2, 12.7, 17.0, 10.9, -1.0, -1.0, 12.4, 16.1, 12.9, 9.6, -1.0, 11.8},    // Distances from Glo Damansara Shopping Mall (G_D_S_M)
  {5.3, 21.6, 3.9, 13.1, 5.8, 12.5, -1.0, -1.0, 18.1, -1.0, 18.1, -1.0, 11.8, 5.4, -1.0, 2.8, -1.0, 12.9, 13.3, -1.0, 15.2, 8.6, 12.4, -1.0, 5.1, 10.8, -1.0, 12.2, 4.9, 5.5, -1.0, 12.2, 5.7, 5.6, 7.8, -1.0, 5.3, 5.7, 6.9, 6.3, 3.3, 12.7, 2.4, 4.9},    // Distances from Great Eastern Mall (G_E_M)
  {14.0, 16.8, -1.0, 7.4, 11.0, 7.0, -1.0, -1.0, 19.3, -1.0, 7.7, 11.7, -1.0, -1.0, -1.0, -1.0, 11.3, 9.3, 10.6, -1.0, 8.4, 13.6, 10.5, -1.0, 12.6, 8.8, -1.0, 5.2, 8.8, 6.8, -1.0, 15.9, 10.1, 7.0, 10.9, 7.9, 9.1, -1.0, 6.3, 13.6, 8.3, 9.7, -1.0, -1.0},    // Distances from Hartamas Shopping Centre (H_S_C)
  {1.7, 14.7, 5.9, 10.4, 2.7, -1.0, -1.0, -1.0, 11.8, -1.0, 17.2, 5.9, 14.2, -1.0, -1.0, 8.9, 2.7, 11.0, 14.6, -1.0, 17.8, 18.1, 10.4, 2.7, -1.0, 7.5, -1.0, 9.4, -1.0, 5.8, -1.0, 8.4, 1.7, 6.3, 15.3, 5.6, 3.6, 3.3, -1.0, 1.3, -1.0, 9.6, 7.3, -1.0},    // Distances from IKEA Cheras (I_C)
  {4.0, 16.1, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 13.1, -1.0, -1.0, 6.3, -1.0, 3.3, -1.0, -1.0, -1.0, -1.0, 11.4, -1.0, -1.0, 14.1, -1.0, -1.0, 3.1, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 9.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0},    // Distances from Imbi Plaza, Kuala Lumpur (I_P_K_L)
  {-1.0, 17.5, 1.7, 10.9, 3.4, 10.1, -1.0, -1.0, 14.7, -1.0, 19.4, 2.6, 9.7, -1.0, -1.0, -1.0, -1.0, 12.3, 12.7, -1.0, 12.8, 9.4, 11.7, -1.0, 3.5, 10.1, -1.0, 10.0, 2.4, 3.4, -1.0, 10.5, 4.2, 3.2, 7.5, -1.0, -1.0, 3.3, 4.5, 4.8, 1.2, 10.9, -1.0, 3.4},    // Distances from Intermark Mall (I_M)
  {-1.0, 14.7, 5.8, 9.5, -1.0, 12.0, -1.0, -1.0, 11.5, -1.0, 14.8, 7.0, 11.8, 2.0, 3.3, 6.1, -1.0, 8.6, 10.0, -1.0, 15.5, 14.7, 8.1, 2.1, -1.0, 7.4, -1.0, 8.6, 5.6, 5.2, -1.0, 7.4, 1.4, 7.5, 13.5, 5.1, 4.7, -1.0, 8.9, 2.5, 5.4, 7.3, 5.1, 4.3},    // Distances from KWC Fashion Wholesale (K_F_W)
  {10.2, 9.7, 9.6, 4.0, 7.9, 12.9, -1.0, -1.0, 11.6, -1.0, -1.0, 15.8, 10.2, 9.5, 7.6, 10.9, 8.5, -1.0, 1.9, -1.0, 14.3, 15.5, 2.0, -1.0, -1.0, 3.5, 10.7, 5.8, 7.5, 6.6, -1.0, 7.0, 8.9, 8.2, 13.6, 6.4, -1.0, -1.0, 9.0, 10.1, 9.5, -1.0, 11.3, 6.6},    // Distances from INC, KL Eco City (I_K_E_C)
  {11.8, 9.6, 11.0, 5.0, -1.0, 13.5, -1.0, -1.0, 11.5, -1.0, 7.2, 17.4, 8.1, 10.9, 9.0, 13.3, 9.2, 3.4, -1.0, -1.0, 14.9, 17.0, 3.6, -1.0, -1.0, 4.9, 10.6, 6.4, 9.0, 8.1, -1.0, 8.6, 10.3, 9.7, 15.1, 7.9, -1.0, -1.0, 10.4, 11.7, 11.0, 4.6, 13.5, 8.1},    // Distances from KL Gateway Mall (K_G_M)
  {4.5, 16.4, 2.1, 8.1, 0.6, 9.5, 0.21, -1.0, 13.4, 0.5, 15.1, 5.8, 9.8, -1.0, -1.0, 4.0, -1.0, 12.3, 11.7, -1.0, 12.4, 12.6, 11.7, -1.0, 3.4, -1.0, 0.8, 9.5, 0.8, 2.7, -1.0, 9.3, 2.4, 3.3, 10.7, -1.0, 0.7, -1.0, 7.9, -1.0, 1.7, 6.7, 3.1, 0.85},    // Distances from Lot 10 (L_10)
  {19.8, 28.7, 15.2, 13.5, 17.2, 7.7, -1.0, -1.0, 25.8, -1.0, 10.6, 17.7, 10.6, 18.9, 17.0, 15.5, 18.3, 18.3, 17.2, -1.0, -1.0, 14.3, 18.0, -1.0, 19.4, 14.6, 15.6, 12.5, 15.6, 13.9, -1.0, 21.7, 16.8, 13.8, 14.7, 14.5, 18.6, -1.0, 13.1, 19.7, 15.1, 15.5, -1.0, 15.4},    // Distances from Lotus's Kepong (L_K)
  {17.1, 30.2, 11.9, 16.9, 14.6, 10.8, -1.0, -1.0, 24.8, -1.0, 19.0, 9.1, 14.0, -1.0, 13.5, 9.8, 15.4, 17.2, 17.6, -1.0, 13.5, -1.0, 16.6, -1.0, 13.7, 15.0, -1.0, 16.0, 12.5, 9.1, -1.0, 20.7, 14.4, 9.0, 3.2, 11.8, 12.9, 13.5, 9.4, 15.0, 11.1, 15.8, 10.1, 10.5},    // Distances from M3 Shopping Mall (M_S_M)
  {11.0, 10.5, 11.1, 4.7, 8.4, 15.7, -1.0, -1.0, 13.2, -1.0, 9.4, 16.6, 10.2, 10.4, 8.2, 13.8, 8.7, 2.1, 2.8, -1.0, 17.1, 18.3, -1.0, 10.8, 13.7, 4.3, -1.0, 8.6, 13.7, 8.1, -1.0, 10.2, 9.0, 9.7, 16.4, 7.9, 12.3, -1.0, 11.8, 10.9, 13.9, 1.1, 12.9, 9.5},    // Distances from Mid Valley Megamall (M_V_M)
  {6.2, 16.3, 7.7, 8.7, 4.1, 13.5, -1.0, -1.0, 12.3, 5.1, 14.2, 10.8, 10.5, 3.1, 3.7, 6.8, 1.1, 7.4, 7.5, -1.0, 14.9, 21.5, 6.9, -1.0, -1.0, 5.1, -1.0, 7.8, 6.7, 6.7, -1.0, 8.1, 2.4, 8.3, 13.5, 6.6, 5.5, 3.8, 9.6, 3.5, 6.9, 6.8, 5.9, 2.4},    // Distances from LaLaport Bukit Bintang City Centre (L_B_B_C_C)
  {2.4, 15.0, 5.3, 9.6, 2.1, 12.1, -1.0, -1.0, 11.6, 3.3, 19.8, 5.3, 11.3, 0.23, 2.2, 4.4, 2.2, 9.6, 13.2, -1.0, 16.4, 13.1, 9.1, 2.2, -1.0, 8.7, -1.0, 8.6, 4.3, 5.5, -1.0, 7.8, 1.2, 8.2, 12.3, 5.3, 3.1, 2.1, 7.6, 1.7, 4.5, 10.4, 3.5, 4.4},    // Distances from MyTOWN Shopping Centre (M_S_C)
  {7.0, 12.8, 5.2, 6.1, 4.3, 12.9, -1.0, -1.0, 15.3, 5.4, 12.9, 13.1, 9.6, 5.4, 3.7, 6.6, 4.5, 5.6, 6.2, -1.0, 15.8, 15.5, 5.0, 5.5, 6.8, -1.0, -1.0, 7.0, -1.0, 7.5, -1.0, 10.8, 4.4, 4.5, 13.4, 4.3, 5.6, -1.0, 6.5, 7.4, 4.5, 4.2, 5.9, 3.4},    // Distances from NU Sentral Shopping Mall (N_S_S_M)
  {16.1, 1.2, 19.5, 19.4, 16.8, 26.0, -1.0, -1.0, 6.5, -1.0, 21.4, 17.8, 22.2, 15.5, 16.8, 18.9, 16.0, 15.3, 14.7, -1.0, 29.0, 36.7, 14.7, 15.8, 14.0, 15.3, -1.0, 20.5, 16.7, 18.7, -1.0, 9.2, 17.0, 22.7, 33.8, 18.6, 18.0, 16.8, 22.1, 16.0, 18.9, 12.0, 17.9, -1.0},    // Distances from Pavillion Bukit Jalil (P_B_J)
  {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.15, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0},    // Distances from Pavillion Damansara Heights (P_D_H)
  {5.1, 16.9, 4.6, -1.0, 1.3, 13.7, -1.0, -1.0, 14.5, -1.0, 15.8, 5.6, 16.2, 3.8, 1.3, 3.7, 2.5, 12.9, 12.3, -1.0, 15.6, 12.3, 12.3, -1.0, 4.0, -1.0, -1.0, 12.8, -1.0, 7.0, -1.0, 9.9, 2.1, 6.3, 10.4, -1.0, 0.45, 1.3, 7.6, 4.6, 3.8, 11.9, 2.8, -1.0},    // Distances from Pavillion Kuala Lumpur (P_K_L)
  {7.4, 13.7, 2.2, 7.5, -1.0, 7.3, -1.0, -1.0, 15.8, -1.0, 14.8, 6.8, 6.9, 6.6, 3.5, 3.7, 5.3, 6.9, 7.9, -1.0, 10.2, 12.4, 6.3, -1.0, 7.3, 4.7, -1.0, 5.3, 2.6, -1.0, -1.0, 14.2, -1.0, 1.4, 7.1, 0.3, -1.0, -1.0, 2.2, 6.7, 2.1, 5.5, 3.2, 1.9},    // Distances from Pertama Complex | First Shopping Center (P_C) 
  {-1.0, -1.0, -1.0, 7.8, -1.0, 10.4, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 9.6, -1.0, 0.081, -1.0, -1.0, 7.5, 7.9, -1.0, 13.2, -1.0, 6.9, -1.0, -1.0, 5.3, -1.0, 6.9, -1.0, -1.0, -1.0, -1.0, 1.6, 4.9, -1.0, 3.6, -1.0, 0.022, 6.0, 3.0, -1.0, 6.1, -1.0, -1.0},    // Distances from Plaza Low Yat @ Bukit Bintang (P_L_Y)
  {9.6, 7.6, 12.2, 11.9, 9.3, 18.5, -1.0, -1.0, 6.2, 10.4, 13.8, 16.5, 14.7, 8.0, 9.0, 11.4, 8.5, 7.8, 7.2, -1.0, 19.9, 20.0, 7.2, 8.3, 8.3, 7.8, -1.0, 12.8, 11.3, 13.3, -1.0, -1.0, 8.0, 12.8, 18.0, 13.1, -1.0, 9.3, 14.6, 9.5, 11.4, 11.7, 10.4, 11.2},    // Distances from Plaza Salak Park (P_S_P)
  {-1.0, 16.2, 4.8, 8.5, 1.7, 11.0, -1.0, -1.0, 13.3, 2.8, -1.0, 5.4, 10.2, 1.1, 2.3, 4.5, 1.8, 8.5, -1.0, -1.0, 13.9, 13.1, -1.0, 1.1, 1.9, 5.9, -1.0, 7.6, 4.4, 4.5, -1.0, 8.1, -1.0, 5.6, 11.1, 4.3, 3.0, 2.3, 6.2, 2.1, 4.4, 6.8, 3.9, 3.3},    // Distances from PUDU PLAZA SHOPPING MALL (P_P_S_M)
  {9.4, 20.9, 2.1, 8.7, 3.7, 7.8, -1.0, -1.0, 18.1, -1.0, 14.1, 6.5, 7.4, 5.8, 6.1, 3.4, 6.9, 10.2, 10.6, -1.0, 10.7, 9.0, 7.9, -1.0, 6.9, 6.3, -1.0, 7.8, 2.5, 1.0, 3.2, 12.4, 4.6, -1.0, 7.1, -1.0, 6.1, -1.0, 2.2, 9.3, 2.0, 7.1, 3.1, 1.8},    // Distances from Quill City Mall Kuala Lumpur (Q_C_M_K_L)
  {16.4, 29.5, 9.7, 13.1, 12.1, 10.7, -1.0, -1.0, 22.3, -1.0, 19.3, 8.4, 13.1, -1.0, 11.0, 7.7, 12.8, 14.6, 15.0, -1.0, 12.6, 2.2, 14.0, -1.0, 11.2, 12.4, -1.0, 12.2, 10.0, 7.0, -1.0, 18.1, 11.9, 6.9, -1.0, 14.3, 10.4, 10.9, 6.9, 12.4, 10.3, 13.3, 9.4, 8.4},    // Distances from Setapak Central Mall (S_C_M)
  {7.7, 13.6, 2.9, 6.2, 3.8, 7.3, -1.0, -1.0, 14.8, -1.0, 10.9, 7.2, 6.8, 6.5, 4.4, 6.3, 5.2, 6.8, 7.8, -1.0, 10.1, 10.1, 6.3, 4.3, 7.7, 4.6, -1.0, 5.3, 3.3, 0.4, -1.0, 10.7, 3.9, 1.5, 8.2, -1.0, -1.0, -1.0, 2.1, 7.6, 2.8, 5.5, -1.0, 2.6},    // Distances from SOGO Kuala Lumpur (S_K_L)
  {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.13, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0},    // Distances from The Starhill (T_S)
  {-1.0, -1.0, 2.2, 10.5, -1.0, 10.2, 0.3, -1.0, -1.0, 0.65, 12.7, -1.0, 9.2, 4.3, 0.65, 3.2, -1.0, 7.6, 8.1, -1.0, 16.1, 12.8, 7.0, -1.0, -1.0, 5.4, -1.0, 7.0, 0.95, 2.8, 0.022, -1.0, 1.7, 3.5, 10.9, -1.0, 0.85, -1.0, 8.1, 3.1, 1.9, 6.2, 3.3, 1.0},    // Distances from Sungei Wang Plaza (S_W_P)
  {9.5, 19.6, 3.9, 8.6, 5.6, 6.5, -1.0, -1.0, 16.5, -1.0, 12.9, 8.3, 6.1, 8.0, 6.2, 5.2, 7.0, 8.6, 9.0, -1.0, 11.6, 8.6, 8.0, -1.0, 8.5, 6.4, -1.0, 7.0, 4.3, 2.7, -1.0, 12.5, 5.7, 2.6, 6.7, 3.4, 7.7, -1.0, -1.0, 9.4, 3.8, 7.2, -1.0, 3.6},    // Distances from Sunway Putra Mall (S_P_M)
  {0.65, 15.4, 6.2, 10.2, 3.1, 15.1, -1.0, -1.0, -1.0, 4.2, 17.1, 8.0, 11.9, -1.0, 3.7, 5.4, 2.8, 11.0, 10.4, -1.0, 17.3, 18.7, 10.4, 2.8, 0.65, 8.9, -1.0, 9.3, -1.0, 6.2, -1.0, 8.0, 2.2, 9.7, 12.0, 7.9, 4.0, 3.6, 9.2, -1.0, 5.4, -1.0, 4.4, -1.0},    // Distances from Sunway Velocity Mall (S_V_M)
  {-1.0, 18.1, 0.9, 9.3, -1.0, 9.3, -1.0, -1.0, 15.3, -1.0, 18.2, 3.8, 8.8, 4.5, -1.0, 1.8, -1.0, 10.0, 11.0, -1.0, 15.3, 11.0, 9.4, 5.6, 4.2, 12.8, -1.0, 12.5, 1.2, 2.5, -1.0, 13.3, 4.8, 3.1, 10.1, -1.0, -1.0, -1.0, 7.3, 5.3, -1.0, 13.6, 1.5, 1.3},    // Distances from Suria KLCC (S_K)
  {11.0, 10.5, 13.9, 4.6, 10.9, 15.8, -1.0, -1.0, 12.5, -1.0, 9.4, 16.6, 10.5, 10.5, 8.3, 13.0, 10.1, 1.3, 2.7, -1.0, 17.1, 16.3, 0.75, 9.8, 13.7, 4.3, -1.0, 6.4, 12.9, 7.4, -1.0, 7.7, 10.4, 9.0, 14.4, 7.2, 11.5, -1.0, 9.7, 10.8, 13.1, -1.0, 12.1, 8.8},    // Distances from The Gardens Mall (T_G_M)
  {5.1, 17.2, 1.9, 11.3, 4.1, 12.4, -1.0, -1.0, 14.3, -1.0, 19.2, 3.2, 10.1, 3.4, 3.0, 1.3, 6.5, 13.1, 12.6, -1.0, 14.3, 10.7, 12.6, -1.0, 3.2, 10.7, -1.0, 10.4, 2.0, 4.5, -1.0, 10.2, 3.9, 3.9, 9.9, -1.0, 2.4, 3.0, 5.1, 4.5, 1.3, 11.5, -1.0, 2.9},    // Distances from The LINC KL (T_L_K)
  {-1.0, 17.8, 1.6, 7.6, 3.3, 9.0, -1.0, -1.0, -1.0, -1.0, 15.3, -1.0, 8.5, 4.2, 2.7, 2.5, -1.0, 7.3, 9.0, -1.0, 11.9, 13.1, -1.0, 3.7, 4.8, 5.1, -1.0, 6.7, 0.9, 2.2, 1.2, 10.7, 3.4, 2.8, 10.8, -1.0, -1.0, 1.3, 5.2, 5.0, 1.2, 5.9, 3.2, -1.0},    // Distances from The Weld | Menara Weld (T_W)
};

static void vSenderTask(void* pvParameters) {
    query_t lValeuToSend;
    BaseType_t xStatus;

    int32_t i = 0;
    lValueToSend = user_query[i];

    for (;;) {
        xStatus = xQueueSendToBack(xQueue, &lValueToSend, 0);

        if (i >= 1000) {
            i = 0;
            lValeuToSend = user_query[0];
        }
        else
            lValeuToSend = user_query[++i];

        if (xStatus != pdPASS) {
            vPrintString("Cannot send to the queue.\r\n");
		}

    }
}




















int main() {


    return 0;
}