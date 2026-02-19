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

int main() {
    key_t key;
    int shmId;
    const char* path = ".";
    int projId = 'S';
    key = ftok(path, projId);
    struct sharedData {
        int turns; // writer is 0 and reader is 1
        int programRunning; // 1 is stoping 0 is running
        char userString[500];
    } *sharedMemoryPtr;
    // char userString[500];

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

    while (sharedMemoryPtr->programRunning != 1) { // fix
        printf("Enter the string: ");
        char *ending = fgets(sharedMemoryPtr->userString, 500, stdin); 
        sharedMemoryPtr->userString[strcspn(sharedMemoryPtr->userString, "\n")] = '\0';
        if (strcmp(sharedMemoryPtr->userString, "quit") == 0) { // This needs to be fixed
            // signal(SIGINT, handleSigint);
            sharedMemoryPtr->programRunning = 1;
            // exit(1); // for now
        } // Also needs to account for EOF and then tell the readers that we are shutting down (gracefully)
        if (ending == NULL) {
            sharedMemoryPtr->programRunning = 1;
            sharedMemoryPtr->turns = 1;
        }
        if (shutdown == 0) {
            sharedMemoryPtr->programRunning = 1;
            sharedMemoryPtr->turns = 1;
        }
        sharedMemoryPtr->turns = 1;
        // sharedMemoryPtr = userString;
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

