#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#define SIZE 4096

int main() {
    key_t key;
    int shmId;
    const char* path = "."
    int projId = 'S';
    key = ftok(path, projId);
    // char *sharedMemoryPtr;
    char userString[500];

    if ((shmId = shmget(key, SIZE, IPC_CREAT|S_IRUSR|S_IWUSR)) < 0) {
        perror("Unable to get shared memory\n");
        exit(1);
    }
    while (userString != "quit") {
        printf("Enter the string: ");
        fgets(userString, 500, stdin);
    }
    return 0;
}

