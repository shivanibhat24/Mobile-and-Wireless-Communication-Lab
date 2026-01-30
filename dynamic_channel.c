#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAX_CHANNELS 100
#define MAX_CLUSTER_SIZE 50

typedef struct {
    int channelId;
    int priority;
} ChannelInfo;

// Function prototypes
int getTotalChannels();
int getValidClusterSize();
bool isValidClusterSize(int N);
void getTrafficDemand(int clusterSize, int* trafficDemand);
void allocateChannels(int clusterSize, int controlChannels, int voiceChannels, int* trafficDemand);
void displayControlChannelMatrix(int clusterSize, int controlChannels);
void displayClusterFairness(int clusterSize, int* trafficDemand);
void normalizeChannelDemand(int clusterSize, int* trafficDemand, int totalAvailableChannels);
void displayVoiceChannelAllocation(int clusterSize, int voiceChannels, int* trafficDemand);
void generateChannelsWithPriority(int voiceChannels, ChannelInfo* channels);
void displayChannelPriorities(ChannelInfo* channels, int voiceChannels);
void allocateTrafficChannels(int clusterSize, ChannelInfo* channels, int voiceChannels, int* trafficDemand, int** trafficMatrix, int maxCols);
void displayTrafficMatrix(int** trafficMatrix, int clusterSize, int* trafficDemand);
void displaySatisfactionMatrix(int clusterSize, int* trafficDemand, int** trafficMatrix);
void displayFairnessDistribution(int clusterSize, ChannelInfo* channels, int voiceChannels, int* trafficDemand, int** trafficMatrix, int maxCols);
void shuffleArray(int* array, int size);
int compareChannels(const void* a, const void* b);
int compareInts(const void* a, const void* b);
int findMax(int* array, int size);
int sumArray(int* array, int size);
int countAllocatedChannels(int* row, int maxCols);

int main() {
    srand(time(NULL)); // Initialize random seed
    
    printf("=== Cellular Channel Allocation System ===\n\n");
    
    int totalChannels = getTotalChannels();
    int clusterSize = getValidClusterSize();
    int controlChannels = (totalChannels + 9) / 10; // Integer ceiling division for 10%
    int voiceChannels = totalChannels - controlChannels;
    
    printf("\n=== Channel Distribution ===\n");
    printf("Total Channels: %d\n", totalChannels);
    printf("Control Channels (10%%): %d\n", controlChannels);
    printf("Voice/Data Channels: %d\n", voiceChannels);
    
    int trafficDemand[MAX_CLUSTER_SIZE];
    getTrafficDemand(clusterSize, trafficDemand);
    allocateChannels(clusterSize, controlChannels, voiceChannels, trafficDemand);
    
    return 0;
}

int getTotalChannels() {
    int channels;
    do {
        printf("Enter the number of total channels (50-100): ");
        if (scanf("%d", &channels) != 1) {
            printf("Invalid input! Please enter a number.\n");
            // Clear input buffer
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        if (channels < 50 || channels > 100) {
            printf("Invalid input! Total channels must be between 50 and 100.\n");
        }
    } while (channels < 50 || channels > 100);
    return channels;
}

int getValidClusterSize() {
    int clusterSize;
    do {
        printf("Enter the cluster size: ");
        if (scanf("%d", &clusterSize) != 1) {
            printf("Invalid input! Please enter a number.\n");
            // Clear input buffer
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        if (clusterSize <= 0 || clusterSize > MAX_CLUSTER_SIZE) {
            printf("Cluster size must be between 1 and %d.\n", MAX_CLUSTER_SIZE);
            continue;
        }
        if (isValidClusterSize(clusterSize)) {
            // Print the success message only once here
            for (int i = 0; i * i <= clusterSize; i++) {
                for (int j = 0; j * j <= clusterSize; j++) {
                    if (i * i + j * j + i * j == clusterSize) {
                        printf("Correct Cluster Size! Value of i = %d and value of j = %d.\n", i, j);
                        return clusterSize;
                    }
                }
            }
        } else {
            printf("Invalid cluster size. Valid cluster sizes follow the pattern N = i² + j² + i*j\n");
            printf("Common valid sizes: 1, 3, 4, 7, 9, 12, 13, 16, 19, 21, 25, 27, 28, 31...\n");
        }
    } while (true);
    return clusterSize;
}

bool isValidClusterSize(int N) {
    if (N <= 0) return false;
    
    for (int i = 0; i * i <= N; i++) {
        for (int j = 0; j * j <= N; j++) {
            if (i * i + j * j + i * j == N) {
                return true;
            }
        }
    }
    return false;
}

void getTrafficDemand(int clusterSize, int* trafficDemand) {
    printf("\nEnter traffic channel demand for each cell:\n");
    for (int i = 0; i < clusterSize; i++) {
        do {
            printf("Enter the number of channels for cell %d: ", i + 1);
            if (scanf("%d", &trafficDemand[i]) != 1) {
                printf("Invalid input! Please enter a number.\n");
                // Clear input buffer
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
                continue;
            }
            if (trafficDemand[i] < 0) {
                printf("Channel demand cannot be negative. Please enter a non-negative number.\n");
            }
        } while (trafficDemand[i] < 0);
    }
}

void allocateChannels(int clusterSize, int controlChannels, int voiceChannels, int* trafficDemand) {
    int totalDemand = sumArray(trafficDemand, clusterSize);
    
    printf("\n=== Channel Allocation Results ===\n");
    printf("Total traffic demand: %d channels\n", totalDemand);
    
    if (totalDemand > voiceChannels) {
        printf("Warning: Total demand (%d) exceeds available voice channels (%d)\n", totalDemand, voiceChannels);
        printf("Some channels will be blocked.\n");
    }
    
    // Display cluster fairness analysis
    displayClusterFairness(clusterSize, trafficDemand);
    
    // Display control channel matrix BEFORE traffic channels
    displayControlChannelMatrix(clusterSize, controlChannels);
    
    // Display voice channel allocation
    displayVoiceChannelAllocation(clusterSize, voiceChannels, trafficDemand);
}

void displayControlChannelMatrix(int clusterSize, int controlChannels) {
    printf("\n=== Control Channel Allocation Matrix ===\n");
    printf("Total control channels: %d\n", controlChannels);
    printf("Distribution across %d cells:\n\n", clusterSize);
    
    int baseChannelsPerCell = controlChannels / clusterSize;
    int extraChannels = controlChannels % clusterSize;
    
    // Create control channel matrix
    int** controlMatrix = (int**)malloc(clusterSize * sizeof(int*));
    if (controlMatrix == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }
    
    int channelId = 1;
    for (int i = 0; i < clusterSize; i++) {
        int channelsForCell = baseChannelsPerCell + (i < extraChannels ? 1 : 0);
        controlMatrix[i] = (int*)calloc(channelsForCell, sizeof(int));
        if (controlMatrix[i] == NULL) {
            printf("Memory allocation failed!\n");
            // Free previously allocated memory
            for (int j = 0; j < i; j++) {
                free(controlMatrix[j]);
            }
            free(controlMatrix);
            return;
        }
        
        // Assign channel IDs
        for (int j = 0; j < channelsForCell; j++) {
            controlMatrix[i][j] = channelId++;
        }
    }
    
    // Display the matrix
    printf("Control Channel Assignment Matrix:\n");
    for (int i = 0; i < clusterSize; i++) {
        int channelsForCell = baseChannelsPerCell + (i < extraChannels ? 1 : 0);
        printf("Cell %2d: [", i + 1);
        for (int j = 0; j < channelsForCell; j++) {
            if (j > 0) printf(", ");
            printf("%2d", controlMatrix[i][j]);
        }
        printf("]\n");
    }
    
    // Free allocated memory
    for (int i = 0; i < clusterSize; i++) {
        free(controlMatrix[i]);
    }
    free(controlMatrix);
}

void displayClusterFairness(int clusterSize, int* trafficDemand) {
    printf("\n=== Cluster Traffic Analysis ===\n");
    int totalDemand = sumArray(trafficDemand, clusterSize);
    int maxDemand = findMax(trafficDemand, clusterSize);
    double avgDemand = (double)totalDemand / clusterSize;
    
    printf("Total demand: %d channels\n", totalDemand);
    printf("Maximum demand (single cell): %d channels\n", maxDemand);
    printf("Average demand per cell: %.2f channels\n", avgDemand);
    
    printf("\nDemand distribution:\n");
    for (int i = 0; i < clusterSize; i++) {
        double percentage = (totalDemand > 0) ? (trafficDemand[i] * 100.0 / totalDemand) : 0.0;
        printf("Cell %d: %d channels (%.1f%%)\n", i + 1, trafficDemand[i], percentage);
    }
}

void normalizeChannelDemand(int clusterSize, int* trafficDemand, int totalAvailableChannels) {
    // This function could be used to normalize demands if total exceeds available
    // For now, we'll keep the original demands for allocation
    printf("\nNote: Original traffic demands will be used for allocation.\n");
    printf("Channels will be allocated based on priority and availability.\n");
}

void displayVoiceChannelAllocation(int clusterSize, int voiceChannels, int* trafficDemand) {
    printf("\n=== Voice Channel Allocation ===\n");
    
    if (voiceChannels <= 0) {
        printf("No voice channels available for allocation.\n");
        return;
    }
    
    ChannelInfo channels[MAX_CHANNELS];
    generateChannelsWithPriority(voiceChannels, channels);
    displayChannelPriorities(channels, voiceChannels);
    
    int maxCols = findMax(trafficDemand, clusterSize);
    if (maxCols == 0) {
        printf("No traffic demand from any cell.\n");
        return;
    }
    
    // Allocate memory for traffic matrix
    int** trafficMatrix = (int**)malloc(clusterSize * sizeof(int*));
    if (trafficMatrix == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }
    
    for (int i = 0; i < clusterSize; i++) {
        trafficMatrix[i] = (int*)calloc(maxCols, sizeof(int));
        if (trafficMatrix[i] == NULL) {
            printf("Memory allocation failed!\n");
            // Free previously allocated memory
            for (int j = 0; j < i; j++) {
                free(trafficMatrix[j]);
            }
            free(trafficMatrix);
            return;
        }
    }
    
    allocateTrafficChannels(clusterSize, channels, voiceChannels, trafficDemand, trafficMatrix, maxCols);
    
    // Display fairness distribution with high/low priority breakdown
    displayFairnessDistribution(clusterSize, channels, voiceChannels, trafficDemand, trafficMatrix, maxCols);
    
    displayTrafficMatrix(trafficMatrix, clusterSize, trafficDemand);
    
    // Display additional metrics
    displaySatisfactionMatrix(clusterSize, trafficDemand, trafficMatrix);
    
    // Free allocated memory
    for (int i = 0; i < clusterSize; i++) {
        free(trafficMatrix[i]);
    }
    free(trafficMatrix);
}

void generateChannelsWithPriority(int voiceChannels, ChannelInfo* channels) {
    int channelNumbers[MAX_CHANNELS];
    for (int i = 0; i < voiceChannels; i++) {
        channelNumbers[i] = i + 1;
    }
    
    shuffleArray(channelNumbers, voiceChannels);
    
    int lowPriorityCount = (voiceChannels * 35 + 99) / 100; // Integer ceiling for 35%
    int highPriorityCount = voiceChannels - lowPriorityCount;
    
    // Assign low priority channels (priority 1-2)
    for (int i = 0; i < lowPriorityCount; i++) {
        channels[i].channelId = channelNumbers[i];
        channels[i].priority = (rand() % 2) + 1;
    }
    
    // Assign high priority channels (priority 3-5)
    for (int i = lowPriorityCount; i < voiceChannels; i++) {
        channels[i].channelId = channelNumbers[i];
        channels[i].priority = (rand() % 3) + 3;
    }
}

void displayChannelPriorities(ChannelInfo* channels, int voiceChannels) {
    int lowPriority[MAX_CHANNELS], highPriority[MAX_CHANNELS];
    int lowCount = 0, highCount = 0;
    
    for (int i = 0; i < voiceChannels; i++) {
        if (channels[i].priority <= 2) {
            lowPriority[lowCount++] = channels[i].channelId;
        } else {
            highPriority[highCount++] = channels[i].channelId;
        }
    }
    
    // Sort arrays using the compareInts function
    qsort(lowPriority, lowCount, sizeof(int), compareInts);
    qsort(highPriority, highCount, sizeof(int), compareInts);
    
    printf("\nChannel Priority Assignment:\n");
    printf("Low Priority Channels (Priority 1-2): %d channels\n", lowCount);
    if (lowCount > 0) {
        printf("Low Priority: ");
        for (int i = 0; i < lowCount; i++) {
            printf("%d ", lowPriority[i]);
        }
        printf("\n");
    }
    
    printf("High Priority Channels (Priority 3-5): %d channels\n", highCount);
    if (highCount > 0) {
        printf("High Priority: ");
        for (int i = 0; i < highCount; i++) {
            printf("%d ", highPriority[i]);
        }
        printf("\n");
    }
}

void allocateTrafficChannels(int clusterSize, ChannelInfo* channels, int voiceChannels, int* trafficDemand, int** trafficMatrix, int maxCols) {
    // Sort channels by priority (highest first)
    qsort(channels, voiceChannels, sizeof(ChannelInfo), compareChannels);
    
    printf("\n=== Fairness Distribution Process ===\n");
    printf("Using priority-based round-robin allocation\n");
    
    int channelIndex = 0;
    for (int col = 0; col < maxCols && channelIndex < voiceChannels; col++) {
        for (int cell = 0; cell < clusterSize && channelIndex < voiceChannels; cell++) {
            if (col < trafficDemand[cell]) {
                trafficMatrix[cell][col] = channels[channelIndex].channelId;
                channelIndex++;
            }
        }
    }
    
    printf("Allocated %d out of %d available voice channels\n", channelIndex, voiceChannels);
}

void displayFairnessDistribution(int clusterSize, ChannelInfo* channels, int voiceChannels, int* trafficDemand, int** trafficMatrix, int maxCols) {
    printf("\n=== Fairness Distribution ===\n");
    printf("High and Low Priority Channel Allocation per Cluster:\n");
    printf("%-6s %-8s %-12s %-12s %-10s\n", 
           "Cell", "Total", "High Priority", "Low Priority", "Demand");
    printf("-----------------------------------------------------\n");
    
    // Create a mapping of channel ID to priority for quick lookup
    int channelPriorityMap[MAX_CHANNELS + 1]; // +1 because channel IDs start from 1
    for (int i = 0; i <= MAX_CHANNELS; i++) {
        channelPriorityMap[i] = 0;
    }
    for (int i = 0; i < voiceChannels; i++) {
        channelPriorityMap[channels[i].channelId] = channels[i].priority;
    }
    
    int totalHighPriority = 0, totalLowPriority = 0, totalDemand = 0;
    
    for (int cell = 0; cell < clusterSize; cell++) {
        int highPriorityCount = 0, lowPriorityCount = 0;
        int totalAllocated = 0;
        
        for (int col = 0; col < trafficDemand[cell]; col++) {
            if (trafficMatrix[cell][col] != 0) {
                totalAllocated++;
                int priority = channelPriorityMap[trafficMatrix[cell][col]];
                if (priority <= 2) {
                    lowPriorityCount++;
                } else {
                    highPriorityCount++;
                }
            }
        }
        
        printf("%-6d %-8d %-12d %-12d %-10d\n", 
               cell + 1, totalAllocated, highPriorityCount, lowPriorityCount, trafficDemand[cell]);
        
        totalHighPriority += highPriorityCount;
        totalLowPriority += lowPriorityCount;
        totalDemand += trafficDemand[cell];
    }
    
    printf("-----------------------------------------------------\n");
    printf("%-6s %-8d %-12d %-12d %-10d\n", 
           "Total", totalHighPriority + totalLowPriority, totalHighPriority, totalLowPriority, totalDemand);
}

void displayTrafficMatrix(int** trafficMatrix, int clusterSize, int* trafficDemand) {
    printf("\n=== Traffic Channel Allocation Matrix ===\n");
    printf("(Channels allocated to each cell)\n");
    
    for (int row = 0; row < clusterSize; row++) {
        printf("Cell %2d: [", row + 1);
        bool first = true;
        for (int col = 0; col < trafficDemand[row]; col++) {
            if (trafficMatrix[row][col] != 0) {
                if (!first) printf(", ");
                printf("%2d", trafficMatrix[row][col]);
                first = false;
            }
        }
        printf("]\n");
    }
}

// Utility functions
void shuffleArray(int* array, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

int compareChannels(const void* a, const void* b) {
    ChannelInfo* channelA = (ChannelInfo*)a;
    ChannelInfo* channelB = (ChannelInfo*)b;
    return channelB->priority - channelA->priority; // Sort in descending order
}

int compareInts(const void* a, const void* b) {
    return (*(int*)a - *(int*)b); // Sort in ascending order
}

int findMax(int* array, int size) {
    if (size <= 0) return 0;
    int max = array[0];
    for (int i = 1; i < size; i++) {
        if (array[i] > max) {
            max = array[i];
        }
    }
    return max;
}

int sumArray(int* array, int size) {
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += array[i];
    }
    return sum;
}

int countAllocatedChannels(int* row, int maxCols) {
    int count = 0;
    for (int i = 0; i < maxCols; i++) {
        if (row[i] != 0) {
            count++;
        }
    }
    return count;
}

void displaySatisfactionMatrix(int clusterSize, int* trafficDemand, int** trafficMatrix) {
    printf("\n=== Performance Analysis ===\n");
    printf("Demand vs Allocation Summary:\n");
    printf("%-6s %-8s %-10s %-15s %-8s %-10s\n", 
           "Cell", "Demand", "Allocated", "Satisfaction%", "Blocked", "Block%");
    printf("---------------------------------------------------------------------\n");
    
    int totalDemand = 0, totalAllocated = 0, totalBlocked = 0;
    
    for (int i = 0; i < clusterSize; i++) {
        int allocated = countAllocatedChannels(trafficMatrix[i], trafficDemand[i]);
        int blocked = trafficDemand[i] - allocated;
        double satisfaction = (trafficDemand[i] > 0) ? (allocated * 100.0 / trafficDemand[i]) : 100.0;
        double blockingPercentage = (trafficDemand[i] > 0) ? (blocked * 100.0 / trafficDemand[i]) : 0.0;
        
        printf("%-6d %-8d %-10d %-15.1f %-8d %-10.1f\n", 
               i + 1, trafficDemand[i], allocated, satisfaction, blocked, blockingPercentage);
        
        totalDemand += trafficDemand[i];
        totalAllocated += allocated;
        totalBlocked += blocked;
    }
    
    printf("---------------------------------------------------------------------\n");
    double overallSatisfaction = (totalDemand > 0) ? (totalAllocated * 100.0 / totalDemand) : 100.0;
    double overallBlocking = (totalDemand > 0) ? (totalBlocked * 100.0 / totalDemand) : 0.0;
    
    printf("%-6s %-8d %-10d %-15.1f %-8d %-10.1f\n", 
           "Total", totalDemand, totalAllocated, overallSatisfaction, totalBlocked, overallBlocking);
}