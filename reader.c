#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define SIZE 4096

struct sharedData {
    int turns; 
    int programRunning; 
    int readersDone;
    char userString[500];
} *sharedMemoryPtr;

int main() {
    key_t key;
    int shmId;
    const char* path = ".";
    int projId = 'S';
    key = ftok(path, projId);
    
    if (key == -1) {
        perror("ftok failed");
        exit(1);
    }
    if ((shmId = shmget(key, SIZE, 0)) < 0) {
        perror("Unable to get shared memory\n");
        exit(1);
    }

    if ((sharedMemoryPtr = shmat(shmId, 0, 0)) == (void*) -1) {
        perror("Unable to attach\n");
        exit(1);
    }

    while (1) {
        while (sharedMemoryPtr->turns != 1) {
            if (sharedMemoryPtr->programRunning == 1) {
                    break;
                }
        }
        if (sharedMemoryPtr->programRunning == 1) {
            break;
        }
        printf("Reader read: %s\n", sharedMemoryPtr->userString);
        sharedMemoryPtr->readersDone++;
    }
    if (shmdt(sharedMemoryPtr) < 0) {
        perror("Unable to detach\n");
        exit(1);
    }
    
    return 0;
}
