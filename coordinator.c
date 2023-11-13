#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>


int main(int argc, char** argv) {
    if (argc != 6) {
        printf("Incorrect number of arguments.\n");
        return -1;
    } else {
        int pids[4];
        int shmids[4];

        for (int i = 2; i < 6; i++) {
            int fd[2];
            pipe(fd); // create pipe | fd[0] is read, fd[1] is write

            pid_t pid  = fork(); // fork process
            pids[i-2] = pid; // store child process pid in an array for later use
            shmids[i-2] = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666); // create shared mem

            if (pid < 0) {
                printf("Fork failed.\n");
            } else if (pid > 0) { // PARENT PROCESS
                printf("Coordinator: forked process with ID %d.\n", pid); // print childs pid
                close(fd[0]);

                write(fd[1], &shmids[i-2], sizeof(shmids[i-2])); // write shmid to pipe
                close(fd[1]); // close pipe

                printf("Coordinator: wrote shm ID %d to pipe (%lu bytes)\n", shmids[i-2], sizeof(shmids[i-2]));

            } else if (pid == 0) { // CHILD PROCESS
                char buffer[sizeof(fd[0])];
                sprintf(buffer, "%d", fd[0]);
                execlp("checker", argv[1], argv[i], buffer, (char *)NULL);
            }
        }



        for (int i = 0; i < 4; i++) {
            printf("Coordinator: waiting for process [%d].\n", pids[i]);
            waitpid(pids[i], NULL, 0);

            int *sharedMemoryPointer = (int*)shmat(shmids[i], NULL, 0);

            if (*sharedMemoryPointer == 0) {
                printf("Coordinator: result %d read from shared memory: %d is not divisible by %d.\n", *sharedMemoryPointer, atoi(argv[i+2]), atoi(argv[1]));
            } else {
                printf("Coordinator: result %d read from shared memory: %d is divisible by %d.\n", *sharedMemoryPointer, atoi(argv[i+2]), atoi(argv[1]));
            }

            shmctl(*sharedMemoryPointer, IPC_RMID, NULL); // destory memory segment
        }


    }

    printf("Coordinator: exiting.\n");
    return 0;
}
