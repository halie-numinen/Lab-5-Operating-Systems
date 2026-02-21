#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>

int main() {
    char *sharedMemoryPtr;
    if ((sharedMemoryPtr = shmat(shmId, 0, 0)) == (void*) -1) {
        perror("Unable to attach\n");
        exit(1);
    }

    if (smdt(sharedMemoryPtr) < 0) {
        perror("Unable to detach\n");
        exit(1);
    }

    if (sharedMemoryPtr->programRunning == 1) {
            break;
        }
    
    return 0;
}
