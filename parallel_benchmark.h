#ifndef PARALLEL_BENCHMARK_H
#define PARALLEL_BENCHMARK_H

/*
Runs a multi-threaded matrix multiplication benchmark, measures the execution
time across different thread counts, and prints a comparison against
the theoretical speedup predicted by Amdahl's Law.
*/

int run_parallel_performance_benchmark(void);

#endif
