#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "cpuInfo.h"
#include "parallel_benchmark.h"

#define N 1024 

typedef struct {
    int *A;
    int *B;
    int *C;
    int start_row;
    int end_row;
} ThreadInfo;

typedef struct {
    double real_time;
    double amdahl_time;
    int thread_count;
} TestResult;

static void* matrix_multiply(void* arg);
static double run_timer(int thread_count);
static double amdahl_calc(double serial_time, int core_count); 
static int test_results_display(TestResult test_results[], int test_count);


//This is the function to use
int run_parallel_performance_benchmark() {
   
    TestResult test_results[10] = {0};

    int core_count = get_count();
    printf("There are %d cores on this machine\n", core_count);

    srand(time(NULL));

    //Run on matrix calculations different amount of threads
    int i = 0;
    int current_threads = 1;
    while (current_threads <= core_count) {
        test_results[i].real_time = run_timer(current_threads);
        test_results[i].thread_count = current_threads;
        current_threads *= 2;
        i++;
    }

    if ((current_threads / 2) != core_count) {
        test_results[i].real_time = run_timer(core_count);
        test_results[i].thread_count = core_count;
    }

    test_results_display(test_results, i);

    return 0;
}

static double amdahl_calc(double base_time, int core_count) {
    double p = 0.9999;

    double speedup = 1.0 / ((1.0 - p) + (p  / (double)core_count));

    return speedup;
}

static int test_results_display(TestResult test_results[], int test_count) {
    printf("\n--- Parallel Performance vs. Amdahl's Law ---\n");

    double base_time = test_results[0].real_time;

    for (int i = 0; i < test_count; i++) {
        int threads = test_results[i].thread_count;
        double time = test_results[i].real_time;
        
        if (threads == 1) {
            // Keep the 1-thread output perfectly aligned with the rest
            printf("1 thread  -> %.2fs (Baseline)\n", time);
        } else {
            // Calculate speedups directly inline to keep the code short
            double speedup = base_time / time;
            double amdahl = amdahl_calc(base_time, threads);
            
            printf("%d threads -> %.2fs (Speedup: %.2fx, Predicted: %.2fx)\n", 
                   threads, time, speedup, amdahl);
        }
    }
    printf("\n");
    return 0;
}

static double run_timer(int thread_count) {
    ThreadInfo t_info[thread_count];
    struct timespec start_time, end_time;

    int* A = malloc(N * N * sizeof(int));
    int* B = malloc(N * N * sizeof(int));
    int* C = (int*)calloc(N * N, sizeof(int));


    for (int i = 0; i < N * N; i++) {
        A[i] = rand() % 11;
        B[i] = rand() %11;
    }

    pthread_t threads[thread_count];
    int rows_per_thread = N / thread_count;
        
    printf("Running test for %d cores\n", thread_count);
    clock_gettime(CLOCK_MONOTONIC, &start_time);

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

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    
    free(A);
    free(B);
    free(C);

    return (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
}

static void* matrix_multiply(void* arg){
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
