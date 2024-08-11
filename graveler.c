#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ROLLS 1000000000// 1000000000
#define MULTI_THREADED 1 // 1: multi-threaded ; 0: single-threaded
#define NB_THREADS 16 // Minimum 1 ; Make sure MAX_ROLLS is divisible by NB_THREADS

#define DEBUG 0

const unsigned maxRolls = MAX_ROLLS/NB_THREADS;
unsigned int thread_result[NB_THREADS] = {0};

#if DEBUG==1
double thread_result_sum_numbers1[NB_THREADS] = {0.0};
#endif // DEBUG

#if MULTI_THREADED==1
const int maxRollsThread = MAX_ROLLS/NB_THREADS;
#else
const int maxRollsThread = MAX_ROLLS;
#endif // MULTI_THREADED

void* rolling_func(int threadId) {
    unsigned int numbers1 = 0;
    unsigned int rolls = 0;
    unsigned int maxOnes = 0;

    srand(time(NULL) + threadId);

    #if DEBUG==1
    printf("DEBUG: T%i: maxRolls = %i\n", threadId, maxRollsThread);
    #endif // DEBUG

    while ((numbers1 < 177) && (rolls < maxRollsThread)) {
        numbers1 = 0;
        for (unsigned int i=1;i<=231;i++) { // 231 rolls
            if (rand() < (RAND_MAX+1u) / 4) { // Has a 1 in 4 chance of being true
                numbers1++;
            }
        }
        if (numbers1 > maxOnes) {
            maxOnes = numbers1;
        }
        rolls++;
        #if DEBUG==1
        //printf("DEBUG: T%i_numbers1 = %i\n",threadId, numbers1);
        thread_result_sum_numbers1[threadId] = (thread_result_sum_numbers1[threadId]*((double)rolls-1.0) + (double)numbers1)/(double)rolls;
        #endif // DEBUG

    }
    #if DEBUG==1
    printf("DEBUG: T%i: maxOnes = %i -L.R.%i\n", threadId, maxOnes, (rand() & 0b11));
    #endif // DEBUG
    thread_result[threadId] = maxOnes;
    return NULL;
}


int main(void)
{
    pthread_t thread_id[NB_THREADS];
    unsigned int maxOnes = 0;
    printf("Program Start!\n");
    printf("Max number of Roll Sessions: %i\n", MAX_ROLLS);

    #if DEBUG==1
    printf("DEBUG MODE\n");
    #endif // DEBUG

    #if MULTI_THREADED==1
    printf("Multi-threaded mode with %i threads\n", NB_THREADS);
    for (int i=0;i<NB_THREADS;i++) {
        pthread_create(&thread_id[i], NULL, rolling_func, i);
    }
    for (int i=0;i<NB_THREADS;i++) {
        pthread_join(thread_id[i], NULL);
        maxOnes = (thread_result[i]>maxOnes)?thread_result[i]:maxOnes;
    }
    #else
    printf("Single-threaded mode\n");
    rolling_func(0); // Single thread
    maxOnes = thread_result[0];
    #endif // MULTI_THREADED

    #if DEBUG==1
    double mean_val_numbers1 = 0;
    for (int k=0;k<NB_THREADS;k++) {
        mean_val_numbers1 += thread_result_sum_numbers1[k];
    }
    mean_val_numbers1 /= (double)(NB_THREADS); // WARNING: division by 0 if numbers1 == 0 at the end for all threads
    printf("DEBUG: Mean value of numbers1 = %f\n", mean_val_numbers1); // Should be around 57.75 (231/4)
    #endif // DEBUG


    printf("\nHighest Ones Roll: %i\n", maxOnes);
    return 0;
}
