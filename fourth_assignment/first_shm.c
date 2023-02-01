#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include<time.h>

int shmid;
int factorial(int n) {
    int result = 1;
    for (int i = 1; i <= n; i++) {
        result *= i;
    }
    return result;
}

//singnal handler function typedef
typedef void(*sighandler_t)(int);

//for releasing shared memory function
void releaseSHM(int signum){
    int status;
    status=shmctl(shmid,IPC_RMID,NULL);//IPC_RMID for removing the shared segment
    if(status==0)
    {
        fprintf(stderr,"Remove shared memory with id=%d\n",shmid);
    }
    else if(status==-1)
    {
        fprintf(stderr,"Cannot remove shared memory withn id=%d\n",shmid);
    }
    else{
        fprintf(stderr,"shmctl() returned wrong value while removing shared memory with id=%d\n",shmid);
    }


    status=kill(0,SIGKILL);
    if(status==0)
    {
        fprintf(stderr,"kill successfull\n");
    }
   else if(status==-1)
    {
        perror("Kill failed");
        fprintf(stderr,"Cannot remove shared memory with id=%d\n",shmid);
    }
    else{
        fprintf(stderr,"kill(2) returned wrong values\n");
}
}

int main() {
    sighandler_t shandler;
    shandler=signal(SIGINT,releaseSHM);
    int pid, status, randomNum, fact;
    int *sharedNum;
    shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0777);
    sharedNum = shmat(shmid, NULL, 0);

    pid = fork();//for creating a child process
 if (pid > 0) {
        // parent process will generate a random number and put in the shared memory segment
        while (1) {
            randomNum = rand() % 10 + 1;
            *sharedNum = randomNum;
            printf("Parent: Put %d in shared memory\n", randomNum);
            sleep(1);
        }
    } else if (pid == 0) {
        // child process calculating the factorial of the given number which is placed in shared memory segment by parent process
        while (1) {
            fact = factorial(*sharedNum);
            printf("Child: %d! = %d\n", *sharedNum, fact);
            sleep(1);
        }
    } else {
        printf("Failed to create child process");
        exit(1);
    }

    // shmdt(sharedNum);
    // shmctl(shmid, IPC_RMID, NULL);

return 0;
}