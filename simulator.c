#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("{\"error\": \"Invalid arguments\"}\n");
        return 1;
    }

    char* algo = argv[1];
    int frameCount = atoi(argv[2]);
    int numPages = argc - 3;

    int* pages = (int*)malloc(numPages * sizeof(int));
    for (int i = 0; i < numPages; i++) {
        pages[i] = atoi(argv[i + 3]);
    }

    int* frames = (int*)malloc(frameCount * sizeof(int));
    int* lastUsed = (int*)malloc(frameCount * sizeof(int)); // For LRU
    
    for (int i = 0; i < frameCount; i++) {
        frames[i] = -1; 
        lastUsed[i] = -1;
    }

    int hits = 0, faults = 0;
    int fifoIndex = 0;
    int currentTime = 0;

    printf("{\"total\": %d, \"steps\": [", numPages);

    for (int i = 0; i < numPages; i++) {
        int page = pages[i];
        bool isHit = false;
        int hitIndex = -1;
        currentTime++;

        // 1. Check if page is already in memory
        for (int j = 0; j < frameCount; j++) {
            if (frames[j] == page) {
                isHit = true;
                hitIndex = j;
                break;
            }
        }

        if (isHit) {
            hits++;
            if (strcmp(algo, "LRU") == 0) {
                lastUsed[hitIndex] = currentTime; 
            }
        } else {
            faults++;
            int replaceIndex = -1;
            
            // 2. Look for an empty frame first
            for (int j = 0; j < frameCount; j++) {
                if (frames[j] == -1) {
                    replaceIndex = j;
                    break;
                }
            }

            // 3. If no empty frames, apply the chosen Algorithm!
            if (replaceIndex == -1) { 
                
                // --- FIFO LOGIC ---
                if (strcmp(algo, "FIFO") == 0) {
                    replaceIndex = fifoIndex;
                    fifoIndex = (fifoIndex + 1) % frameCount; 
                } 
                
                // --- LRU LOGIC ---
                else if (strcmp(algo, "LRU") == 0) {
                    int minTime = currentTime + 1;
                    for (int j = 0; j < frameCount; j++) {
                        if (lastUsed[j] < minTime) {
                            minTime = lastUsed[j];
                            replaceIndex = j;
                        }
                    }
                } 
                
                // --- OPTIMAL LOGIC ---
                else if (strcmp(algo, "Optimal") == 0) {
                    int farthest = -1;
                    for (int j = 0; j < frameCount; j++) {
                        int nextUse = -1;
                        // Look into the FUTURE (from i+1 to end)
                        for (int k = i + 1; k < numPages; k++) {
                            if (frames[j] == pages[k]) {
                                nextUse = k;
                                break;
                            }
                        }
                        // If page is never used again in the future, replace it immediately
                        if (nextUse == -1) {
                            replaceIndex = j;
                            break;
                        }
                        // Otherwise, find the one used farthest in the future
                        if (nextUse > farthest) {
                            farthest = nextUse;
                            replaceIndex = j;
                        }
                    }
                }
            }

            frames[replaceIndex] = page;
            lastUsed[replaceIndex] = currentTime;
        }

        // Print JSON step
        printf("{ \"page\": %d, \"status\": \"%s\", \"frames\": [", page, isHit ? "HIT" : "FAULT");
        for (int j = 0; j < frameCount; j++) {
            if (frames[j] != -1) printf("%d", frames[j]);
            else printf("null");
            if (j < frameCount - 1) printf(", ");
        }
        printf("] }");
        if (i < numPages - 1) printf(", ");
    }

    printf("], \"hits\": %d, \"faults\": %d}\n", hits, faults);

    free(pages); free(frames); free(lastUsed);
    return 0;
}