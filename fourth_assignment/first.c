#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include<sys/types.h>
#include<time.h>
#define INTERVAL 2 // interval in seconds between updates

int factorial(int n) {
    int result = 1;
    for (int i = 1; i <= n; i++) {
        result *= i;
    }
    return result;
}

int main() {
    int shm_id;
    int *shared_num;
    int status;

    // create shared memory segment
    shm_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0777);
    if (shm_id < 0) {
        perror("Error creating shared memory segment");
        exit(1);
    }
  // attach shared memory segment to process
    shared_num = (int *) shmat(shm_id, NULL, 0);
    if (shared_num == (int *) -1) {
        perror("Error attaching shared memory segment");
        exit(1);
    }
while(1){
    pid_t pid = fork();
    if (pid == 0) {
        // child process
            printf("Child: Factorial of %d is %d\n", *shared_num, factorial(*shared_num));
            //sleep(INTERVAL);
            exit(1);
    } 
    else {
        // parent process
   //wait(&status);
             srand(time(NULL));
            *shared_num = rand() % 10 + 1; // generate random number between 1 and 10
             printf("Parent: Generated random number %d\n", *shared_num);
            //sleep(INTERVAL);
    }
}
    // detach and release shared memory segment
    shmdt(shared_num);
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}
