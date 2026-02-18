#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#define SIZE 4096

volatile sig_atomic_t programRunning = 1;

void handleSigint(int sig) { // helpes with garaceful shutdown
    if (sig == SIGINT) {
        programRunning = 0;
    }
}

int main() {
    key_t key;
    int shmId;
    const char* path = ".";
    int projId = 'S';
    key = ftok(path, projId);
    char *sharedMemoryPtr;
    char userString[500];

    if (key == -1) {
        perror("ftok failed");
        exit(1);
    }

    if ((shmId = shmget(key, SIZE, IPC_CREAT|S_IRUSR|S_IWUSR)) < 0) {
        perror("Unable to get shared memory\n");
        exit(1);
    }

    signal(SIGINT, handleSigint);
    if ((sharedMemoryPtr = shmat(shmId, 0, 0)) == (void*) -1) { // Need to cordinate with readers somehow
        perror("Unable to attach\n");
        exit(1);
    }

    while (programRunning != 0) {
        printf("Enter the string: ");
        fgets(userString, 500, stdin); // this needs to be added to shared memory
        if (userString == "quit") { // This needs to be fixed
            signal(SIGINT, handleSigint);
            // exit(1); // for now
        } // Also needs to account for EOF and then tell the readers that we are shutting down (gracefully)
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

