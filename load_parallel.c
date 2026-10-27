#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "cpuInfo.h"

#define N 1024 

void* matrix_multiply(void* arg);
int run_timer(int thread_count);
int amdahl_calc(time_t serial_time, int core_count); 

typedef struct {
    int *A;
    int *B;
    int *C;
    int start_row;
    int end_row;
} ThreadInfo;

int main() {


    int core_count = get_count();
    printf("There are %d cores on this machine", core_count);

    srand(time(NULL));

    //Run on different amount of threads
    int current_threads = 1;
    while (current_threads <= core_count) {
        run_timer(current_threads);
        current_threads *= 2;
    }

    if ((current_threads / 2) != core_count) {
        run_timer(current_threads);
    }



}

int amdahl_calc(time_t serial_time, int core_count) {
    return 0;
}

int run_timer(int thread_count) {
    ThreadInfo t_info[thread_count];

    int* A = malloc(N * N * sizeof(int));
    int* B = malloc(N * N * sizeof(int));
    int* C = (int*)calloc(N * N, sizeof(int));


    for (int i = 0; i < N * N; i++) {
        A[i] = rand() % 11;
        B[i] = rand() %11;
    }

    pthread_t threads[thread_count];
    int rows_per_thread = N / thread_count;

    for (int i = 0; i < thread_count; i++) {
        t_info[i].A = A;
        t_info[i].B = B;
        t_info[i].C = C;
        t_info[i].start_row = i * rows_per_thread;
        t_info[i].end_row = (i + 1) * rows_per_thread;
        pthread_create(&threads[i], NULL, matrix_multiply, &t_info[i]);
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("DONE!");
    return 0;
}

void* matrix_multiply(void* arg){
    ThreadInfo *info = (ThreadInfo*)arg;

    for (int i = info->start_row; i < info->end_row; i++) {
        for (int j = 0; j < N; j++) {
            int sum = 0;
            for (int k = 0; k < N; k++) {
                sum += info->A[i * N + k] * info->B[k * N + j];
            }
            info->C[i * N + j] = sum;
        }
    }

    return NULL;
}
