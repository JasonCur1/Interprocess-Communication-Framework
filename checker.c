#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main (int argc, char** argv) {
    int divisor = atoi(argv[0]);
    int dividend = atoi(argv[1]);
    int pid = getpid();
    int fd = atoi(argv[2]); // read end of pipe passed through cmd line argument
    int shmid = atoi(argv[2]);

    printf("Checker process [%d]: starting.\n", pid);

    read(fd, &shmid, sizeof(shmid)); // read shmid from pipe
    close(fd); // close read end of pipe

    printf("Checker process [%d]: read %lu bytes containing shm ID %d.\n", pid, sizeof(shmid), shmid);

    int *sharedMemoryPointer = (int*)shmat(shmid, NULL, 0); // get pointer to shared memory using shmid

    if (dividend % divisor == 0) {
        printf("Checker process [%d]: %d *IS* divisible by %d.\n", pid, dividend, divisor);
        *sharedMemoryPointer = 1; // write to shared memory
        shmdt(sharedMemoryPointer); // detach from shared mem
        printf("Checker process [%d]: Wrote result (1) to shared memory.\n", pid);
        exit(0);
    } else {
        printf("Checker process [%d]: %d *IS NOT* divisible by %d.\n", pid, dividend, divisor);
        *sharedMemoryPointer = 0; // write to shared memory
        shmdt(sharedMemoryPointer); // detach from shared mem
        printf("Checker process [%d]: Wrote result (0) to shared memory.\n", pid);
        exit(0);
    }
}
