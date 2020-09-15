#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>


/*
 * TOTAL_SAMPLES must be divisible by NUMBER_OF_CONCURRENT_PROCESSES
 */
#define  HISTOGRAM_HALF_WIDTH 12
#define  TOTAL_SAMPLES 500000
#define  NUMBER_OF_CONCURRENT_PROCESSES 5

int getSample();

void printHistogram(int *histogram);


sem_t mutex;  //a semaphore for mutual exclusion

int main() {

    sem_init(&mutex, 0, 1); //initialize semaphore
    srand(time(NULL));

    pid_t pid;
    int sharedMemoryId = shmget(ftok("Ali", 65), (HISTOGRAM_HALF_WIDTH * 2) + 1 * sizeof(int), 0666 | IPC_CREAT);


    clock_t startTime = clock();

    for (int i = 0; i < NUMBER_OF_CONCURRENT_PROCESSES; ++i) {
        pid = fork();
        if (pid == 0) {
            // Child Process
            int *hist = (int *) shmat(sharedMemoryId, (void *) 0, 0);
            for (int s = 0; s < TOTAL_SAMPLES / NUMBER_OF_CONCURRENT_PROCESSES; ++s) {
                sem_wait(&mutex);
                ++hist[getSample() + HISTOGRAM_HALF_WIDTH];
                sem_post(&mutex);
            }
            shmdt(hist);
            exit(0);
        } else if (pid == -1) {
            printf("Error in forking a process");
        }
    }


    /*
     * waiting for all children of this process
     * Note: if this process doesn't have any child then wait function return -1 immediately.
     */
    int status;
    while ((pid = waitpid(-1, &status, 0)) != -1) {
        printf("Child process %d terminated\n", pid);
    }


    clock_t endTime = clock();


    printf("Execution time for sample size %d is %f seconds\n", TOTAL_SAMPLES,
           ((double) (endTime - startTime)) / CLOCKS_PER_SEC);

    int *hist = (int *) shmat(sharedMemoryId, (void *) 0, 0);
    printHistogram(hist);

    shmdt(hist);
    shmctl(sharedMemoryId, IPC_RMID, NULL);

    sem_destroy(&mutex);
    return 0;
}

/*
 * this function return an integer as sample which picked from range -HISTOGRAM_HALF_WIDTH to +HISTOGRAM_HALF_WIDTH
 * because of algorithm that I used -->{
 *                                          1. HISTOGRAM_HALF_WIDTH is odd then samples are odd integers.
 *                                          2. HISTOGRAM_HALF_WIDTH is even then samples are even integers.
 *                                      }
 */
int getSample() {
    int sample = 0;
    for (int step = 0; step < HISTOGRAM_HALF_WIDTH; ++step) {
        sample += (rand() % 100) <= 49 ? -1 : 1;
    }
    return sample;
}

/*
 * this function prints histogram (this is not really histogram :D )
 */
void printHistogram(int *histogram) {
    for (int sample = 0; sample < (HISTOGRAM_HALF_WIDTH * 2 + 1); ++sample) {
        printf("%d\t:\t %d\n", sample - HISTOGRAM_HALF_WIDTH, histogram[sample]);
    }
}
