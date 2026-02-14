#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define FOO 4096

int main () {
    struct shmid_ds buf;

    int shmId;
    char *sharedMemoryPtr;

    if((shmId = shmget(IPC_PRIVATE, FOO, IPC_CREAT|S_IRUSR|S_IWUSR)) < 0) {
        perror ("Unable to get shared memory\n");
        exit (1);
    }
    if((sharedMemoryPtr = shmat(shmId, 0, 0)) == (void*) -1) {
        perror ("Unable to attach\n");
        exit (1);
    }
 
    //printf("Value a: %p\t, Value a size: %zu Value b: %p\n, Value b size: %zu", (void *) sharedMemoryPtr, segment_size, (void *) sharedMemoryPtr + FOO, segment_size);

    if (shmctl(shmId, IPC_STAT, &buf) == -1) {
        perror("shctl(IPC_STAT) filed\n");
        exit(1);
    }
    else {
        printf("Value a: %p\t, Value b: %p\nSize of shared memory: %zu bytes\n", (void *) sharedMemoryPtr, (void *) sharedMemoryPtr + FOO, buf.shm_segsz);
        pause();
    }

    if(shmdt(sharedMemoryPtr) < 0) {
        perror ("Unable to detach\n");
        exit (1);
    }
    if(shmctl(shmId, IPC_RMID, 0) < 0) {
        perror("Unable to deallocate\n");
        exit(1);
    }
 
    return 0;
}
