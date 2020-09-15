#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define  HISTOGRAM_HALF_WIDTH 12
#define  TOTAL_SAMPLES 500

int getSample();

void printHistogram(int *histogram);

int main() {
    srand(time(NULL));

    int hist[(HISTOGRAM_HALF_WIDTH * 2) + 1] = {0};

    clock_t startTime = clock();
    for (int sizeOfSamples = 0; sizeOfSamples < TOTAL_SAMPLES; ++sizeOfSamples) {
        ++hist[getSample() + HISTOGRAM_HALF_WIDTH];
    }
    clock_t endTime = clock();
    printf("Execution time for sample size %d is %f seconds\n", TOTAL_SAMPLES,
           ((double) (endTime - startTime)) / CLOCKS_PER_SEC);

    printHistogram(hist);
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