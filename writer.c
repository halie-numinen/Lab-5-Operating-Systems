#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define SIZE 4096

volatile sig_atomic_t shutdown = 1;

void handleSigint(int sig) { // helpes with garaceful shutdown
    if (sig == SIGINT) {
        shutdown = 0;
    }
}

struct sharedData {
    int turns; // 0 = writer's turn, 1 = reader's turn
    int programRunning; // 0 = running, 1 = stopping
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

    if ((shmId = shmget(key, SIZE, IPC_CREAT|S_IRUSR|S_IWUSR)) < 0) {
        perror("Unable to get shared memory\n");
        exit(1);
    }

    signal(SIGINT, handleSigint);
    if ((sharedMemoryPtr = (struct sharedData *) shmat(shmId, 0, 0)) == (void*) -1) { // Need to cordinate with readers somehow
        perror("Unable to attach\n");
        exit(1);
    }

    sharedMemoryPtr->turns = 0;
    sharedMemoryPtr->programRunning = 0;
    sharedMemoryPtr->readersDone = 0;

    while (1) { 
        while (sharedMemoryPtr->turns != 0) { // when it is the writers turn
            if (sharedMemoryPtr->programRunning == 1) {
                break;
            }
        }
        if (sharedMemoryPtr->programRunning == 1) {
            break;
        }

        printf("Enter the string: ");
        char *ending = fgets(sharedMemoryPtr->userString, 500, stdin); 
        if (ending == NULL) {
            sharedMemoryPtr->programRunning = 1;
            sharedMemoryPtr->turns = 1;
            break;
        }
        sharedMemoryPtr->userString[strcspn(sharedMemoryPtr->userString, "\n")] = '\0';
        
        if (strcmp(sharedMemoryPtr->userString, "quit") == 0 || shutdown == 0) { 
            sharedMemoryPtr->programRunning = 1;
            sharedMemoryPtr->turns = 1;
            break;
        } 
        
        sharedMemoryPtr->readersDone = 0;
        sharedMemoryPtr->turns = 1;
        while (sharedMemoryPtr->readersDone < 2) {
            if (sharedMemoryPtr->programRunning == 1) {
                break;
            }
        }
        sharedMemoryPtr->turns = 0;
    }
    
    if (shmdt(sharedMemoryPtr) < 0) {
        perror("Unable to detach\n");
        exit(1);
    }
    if (shmctl(shmId, IPC_RMID, 0) < 0) {
        perror("Unable to deallocate\n");
        exit(0);
    }
    return 0;
}

