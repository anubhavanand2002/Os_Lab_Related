#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define MAX_ROWS 10
#define MAX_COLS 10

int a[MAX_ROWS][MAX_COLS], b[MAX_ROWS][MAX_COLS], c[MAX_ROWS][MAX_COLS];

int main() {
    int n, m, p;

    printf("Enter the number of rows for matrix A: ");
    scanf("%d", &n);
    printf("Enter the number of columns for matrix A and rows for matrix B: ");
    scanf("%d", &m);
    printf("Enter the number of columns for matrix B: ");
    scanf("%d", &p);

    printf("Enter elements of matrix A:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            scanf("%d", &a[i][j]);
        }
    }

    printf("Enter elements of matrix B:\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            scanf("%d", &b[i][j]);
        }
    }
    
    //creating a shared memory segment by using a shmget which returna a id corresponding to it
    int shm_id = shmget(IPC_PRIVATE, sizeof(int) * n * p, IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("Error creating shared memory segment for matrix C");
        exit(1);
    }
    
    //trying to create a pointer corresponding to that created shared memory segment
    int *shared_c = (int *) shmat(shm_id, NULL, 0);
    if (shared_c == (int *) -1) {
        perror("Error attaching shared memory segment for matrix C");
        exit(1);
    }
    

    //trying to calculate the result of matrix multiplication of each row by a child by calling fork() system call
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // child process
            int row_start = i * p;
            for (int j = 0; j < p; j++) {
                int sum = 0;
                for (int k = 0; k < m; k++) {
                    sum += a[i][k] * b[k][j];
                }
                shared_c[row_start + j] = sum;
            }
            exit(0);
        }
    }

    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    //storing that result in matrix c
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            c[i][j] = shared_c[i * p + j];
        }
    }
    
    shmdt(shared_c);
    shmctl(shm_id, IPC_RMID, NULL);
    printf("product of matrix A and B :\n");
    for(int i = 0; i < n; i++){
        for(int j = 0; j < p; j++){
            printf("%d  ", c[i][j]);
        }
        puts("");
    }
    return 0;
}