#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define FOO 4096

struct shmid_ds buf;
size_t segment_size;
int main ()
{
 int shmId;
 char *sharedMemoryPtr;

 if((shmId = shmget(IPC_PRIVATE, FOO, IPC_CREAT|S_IRUSR|S_IWUSR)) < 0) {
 perror ("Unable to get shared memory\n");
 exit (1);
 }
 if((sharedMemoryPtr = shmat (shmId, 0, 0)) == (void*) -1) {
 perror ("Unable to attach\n");
 exit (1);
 }
 
 segment_size = buf.shm_segsz;
 //printf("Value a: %p\t, Value a size: %zu Value b: %p\n, Value b size: %zu", (void *) sharedMemoryPtr, segment_size, (void *)
//sharedMemoryPtr + FOO, segment_size);

 if(shmdt (sharedMemoryPtr) < 0) {
 perror ("Unable to detach\n");
 exit (1);
 }
 if(shmctl (shmId, IPC_RMID, 0) < 0) {
 perror ("Unable to deallocate\n");
 exit(1);
 }
 
 printf("Value a: %p\t, Value a size: %zu Value b: %p\n, Value b size: %zu", (void *) sharedMemoryPtr, segment_size, (void *)
sharedMemoryPtr + FOO, segment_size);
 return 0;
}
