#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <math.h> 
#define MIN_CHANNELS 50 
#define MAX_CHANNELS 100 
#define MIN_CONTROL_PERCENTAGE 0.10 
#define MAX_CONTROL_PERCENTAGE 0.15 
/** 
* @brief Prints a dynamically allocated 2D matrix. 
* * @param rows The number of rows in the matrix. 
* @param cols The number of columns in the matrix. 
* @param matrix A pointer to the 2D array. 
* @param title A string containing the title for the matrix. 
 */ 
void printMatrix(int rows, int cols, int** matrix, const char* title) { 
    printf("\n%s is:\n", title); 
    for (int i = 0; i < rows; i++) { 
        printf("["); 
        for (int j = 0; j < cols; j++) { 
            // Check if the element is not zero to avoid printing unused slots 
            if (matrix[i][j] != 0) { 
                printf("%d ", matrix[i][j]); 
            } 
        } 
        printf("]\n"); 
    } 
} 
 
int main() { 
    int totalChannels; 
    int clusterSize; 
    int controlChannelsCount; 
    int trafficChannelsCount; 
    double controlChannelPercentage; 
 
    // Prompt the user for the total number of channels 
    printf("Enter the number of total channels (50-100): "); 
    scanf("%d", &totalChannels); 
 
    // Validate the input for total channels 
    if (totalChannels < MIN_CHANNELS || totalChannels > MAX_CHANNELS) { 
        printf("Invalid number of total channels. Please enter a value between %d and %d.\n", MIN_CHANNELS, 
MAX_CHANNELS); 
        return 1; 
    } 
 
    // Prompt the user for the cluster size 
    printf("Enter the cluster size (7, 9, or 13): "); 
    scanf("%d", &clusterSize); 
 
    // Validate the input for cluster size 
    if (clusterSize != 7 && clusterSize != 9 && clusterSize != 13) { 
        printf("Invalid cluster size. Please enter 7, 9, or 13.\n"); 
        return 1; 
    } 
 
    // Prompt the user for the control channel percentage 
    printf("Enter the percentage of control channels (e.g., 0.10 for 10%%): "); 
    scanf("%lf", &controlChannelPercentage); 
 
    // Validate the input for control channel percentage 
    if (controlChannelPercentage < MIN_CONTROL_PERCENTAGE || controlChannelPercentage > 
MAX_CONTROL_PERCENTAGE) { 
        printf("Invalid control channel percentage. Please enter a value between %.2f and %.2f.\n", 
MIN_CONTROL_PERCENTAGE, MAX_CONTROL_PERCENTAGE); 
        return 1; 
    } 
 
    // Calculate the number of control and traffic channels based on the user-provided percentage 
    controlChannelsCount = (int)ceil(totalChannels * controlChannelPercentage); 
 
    // Ensure there is at least one control channel per cell in the cluster 
    if (controlChannelsCount < clusterSize) { 
        controlChannelsCount = clusterSize; 
    } 
    trafficChannelsCount = totalChannels - controlChannelsCount; 
 
    printf("\nOutput for case: Total channels = %d and cluster size = %d\n", totalChannels, clusterSize); 
 
    // Calculate matrix dimensions for control channels 
    int controlMatrixRows = clusterSize; 
    // Use ceiling division to determine the number of columns needed 
    int controlMatrixCols = (int)ceil((double)controlChannelsCount / clusterSize); 
 
    // Dynamically allocate memory for the control channel matrix 
    int** controlMatrix = (int**)malloc(controlMatrixRows * sizeof(int*)); 
    if (controlMatrix == NULL) { 
        printf("Memory allocation failed for control matrix.\n"); 
        return 1; 
    } 
    for (int i = 0; i < controlMatrixRows; i++) { 
        controlMatrix[i] = (int*)malloc(controlMatrixCols * sizeof(int)); 
        if (controlMatrix[i] == NULL) { 
            printf("Memory allocation failed for control matrix row %d.\n", i); 
            return 1; 
        } 
        // Initialize the memory to zero 
        memset(controlMatrix[i], 0, controlMatrixCols * sizeof(int)); 
    } 
 
    // Calculate matrix dimensions for traffic channels 
    int trafficMatrixRows = clusterSize; 
    int trafficMatrixCols = (int)ceil((double)trafficChannelsCount / clusterSize); 
 
    // Dynamically allocate memory for the traffic channel matrix 
    int** trafficMatrix = (int**)malloc(trafficMatrixRows * sizeof(int*)); 
    if (trafficMatrix == NULL) { 
        printf("Memory allocation failed for traffic matrix.\n"); 
        return 1; 
    } 
    for (int i = 0; i < trafficMatrixRows; i++) { 
        trafficMatrix[i] = (int*)malloc(trafficMatrixCols * sizeof(int)); 
        if (trafficMatrix[i] == NULL) { 
            printf("Memory allocation failed for traffic matrix row %d.\n", i); 
            return 1; 
        } 
        // Initialize the memory to zero 
        memset(trafficMatrix[i], 0, trafficMatrixCols * sizeof(int)); 
    } 
 
    // Distribute control channels in a round-robin fashion across cells (rows) 
    for (int i = 0; i < controlChannelsCount; i++) { 
        int row = i % clusterSize; 
        int col = i / clusterSize; 
        if (row < controlMatrixRows && col < controlMatrixCols) { 
            controlMatrix[row][col] = i + 1; // Assign channel numbers from 1 
        } 
    } 
 
    // Distribute traffic channels sequentially after the control channels 
    for (int i = 0; i < trafficChannelsCount; i++) { 
        int row = i % clusterSize; 
        int col = i / clusterSize; 
        if (row < trafficMatrixRows && col < trafficMatrixCols) { 
            trafficMatrix[row][col] = controlChannelsCount + 1 + i; // Correctly start after the last control channel 
        } 
    } 
 
    // Print the resulting matrices 
    printMatrix(controlMatrixRows, controlMatrixCols, controlMatrix, "The control channel matrix"); 
    printMatrix(trafficMatrixRows, trafficMatrixCols, trafficMatrix, "The traffic channel matrix"); 
 
    // Free the dynamically allocated memory to prevent memory leaks 
    for (int i = 0; i < controlMatrixRows; i++) { 
        free(controlMatrix[i]); 
    } 
    free(controlMatrix); 
 
    for (int i = 0; i < trafficMatrixRows; i++) { 
free(trafficMatrix[i]); 
} 
free(trafficMatrix); 
return 0; 
}