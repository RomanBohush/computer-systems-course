#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define N 1000000L
#define THREADS 4

typedef struct {
    long start;
    long end;
    long result;
} Task;

void *calculate_sum(void *arg)
{
    Task *task = arg;

    long sum = 0;

    for (long i = task->start; i <= task->end; i++) {
        sum += i;
    }

    task->result = sum;

    return NULL;
}

int main(void)
{
    pthread_t threads[THREADS];
    Task tasks[THREADS];
    long chunk = N / THREADS;

    for (int i = 0; i < THREADS; i++) {
        tasks[i].start = i * chunk + 1;
        tasks[i].end = (i + 1) * chunk;
        tasks[i].result = 0;

        if (pthread_create(&threads[i], NULL, calculate_sum, &tasks[i]) != 0) {
            fprintf(stderr, "pthread_create failed\n");
            return EXIT_FAILURE;
        }
    }

    long total = 0;

    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
        printf("Thread %d: %ld..%ld = %ld\n",
               i + 1, tasks[i].start, tasks[i].end, tasks[i].result);
        total += tasks[i].result;
    }

    long expected = N * (N + 1) / 2;

    printf("\nParallel result: %ld\n", total);
    printf("Expected result: %ld\n", expected);
    printf("Correct: %s\n", total == expected ? "YES" : "NO");

    return EXIT_SUCCESS;
}
