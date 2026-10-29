/*
 *
 * Experiment 1: pointer chase over a shuffled cyclic list of growing size
 *                getting time per access vs working-set size 
 *
 * Experiment 2: strided walk over a large array with growing stride
 *               getting us time per access vs stride 
 *
 * Saves what /sys says about cpu0's caches for comparison
 * 
 *
 *
 *Outputs files:
 * results.csv    experiment,x_bytes,ns_per_access
 * sys_cache.csv  index,level,type,size,line_size_bytes
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ACCESSES 10000000L /* accesses per timed run */
#define REPEATS   3        /* num of runs kept, more runs is more noisy */

volatile size_t  sink;    /* stops the compiler deleting our loops */



/* current time in nanoseconds */
double now_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e9 + ts.tv_nsec;
}

 

/* ------- /sys cache info ---------- */
 

/* read the first line of one file under .../cache/indexN/ into buf */

void read_cache_file(int index, const char *name, char *buf, int size)
{
    char path[200];
    FILE *f;

    buf[0] = '\0';
    sprintf(path, "/sys/devices/system/cpu/cpu0/cache/index%d/%s", index, name);
    f = fopen(path, "r");
    if (f == NULL) {
        return;
    }
    if (fgets(buf, size, f) == NULL) {
        buf[0] = '\0';
    }
    fclose(f);

    /* strip trailing newline */
    for (int i = 0; buf[i] != '\0'; i++) {
        if (buf[i] == '\n') {
            buf[i] = '\0';
        }
    }
}




/* write what /sys reports for cpu0's caches to a CSV file */
void save_sys_info(const char *filename)
{
    char level[64], type[64], size[64], line[64];
    FILE *out = fopen(filename, "w");
    if (out == NULL) {
        perror(filename);
        exit(1);
    }

    fprintf(out, "index,level,type,size,line_size_bytes\n");
    for (int i = 0; i < 8; i++) {
        read_cache_file(i, "level", level, sizeof(level));
        if (level[0] == '\0') {
            break;   /* no more cache indexes */
        }
        read_cache_file(i, "type", type, sizeof(type)); 
        read_cache_file(i, "size", size, sizeof(size));
        read_cache_file(i, "coherency_line_size", line, sizeof(line));
        fprintf(out, "%d,%s,%s,%s,%s\n", i, level, type, size, line);
    }
    fclose(out);
    printf("Saved %s\n", filename);
}



/* ---------- Experiment 1: working-set size ---------- */

/* returns the best time per access (ns) for a working set of 'bytes' */
double time_chase(size_t bytes)
{
    size_t n =  bytes / sizeof(size_t);         /* number of elements */
    size_t *arr = malloc(n * sizeof(size_t));
    size_t *order = malloc(n * sizeof(size_t));
    if (arr == NULL || order == NULL) {
        perror("malloc");
        exit(1);
    }

    /* order = 0, 1, 2, ..., n-1 */
    for (size_t i = 0; i < n; i++) {
        order[i] = i;
    }


    /* Fisher-Yates shuffle of the visiting order */
    for (size_t i = n - 1; i > 0; i--) {
        size_t j =  rand() % (i + 1);
        size_t tmp = order[i];
        order[i] = order[j];
        order[j] = tmp; 
    }

    /* link each element to the next one in the shuffled order (one cycle) */
    for (size_t k = 0; k < n; k++) {
        arr[order[k]] = order[(k + 1) % n];
    }

    /* warm-up: walk the whole cycle once so the data is in cache */
    size_t p = 0;
    for (size_t i = 0; i < n; i++) {
        p = arr[p];
    }

    /* timed runs, keep the fastest */
    double best = 1e18;
    for (int r = 0; r < REPEATS; r++) {
        double start = now_ns();
        for (long i = 0; i < ACCESSES; i++) {
            p = arr[p];
        } 
        double end = now_ns();

        double per_access = (end - start) / ACCESSES;
        if (per_access < best) {
            best = per_access;
        }
    }

    sink = p;    /* use the result so the loop is not removed */

    free(arr);
    free(order);
    return best;
}




/* ---------- Experiment 2: stride ---------- */

/* returns the best time per access (ns) walking a big array with a stride */
double time_stride(size_t bytes, size_t stride)
{
    unsigned char *arr = malloc(bytes);
    if (arr == NULL) {
        perror("malloc");
        exit(1);
    }

    /* write to every byte so the OS gives us real memory */
    for (size_t i = 0; i < bytes; i++) {
        arr[i] = 1;
    }



    /* warm-up pass */
    unsigned long sum = 0;
    size_t pos = 0;
    for (long i = 0;  i < ACCESSES; i++) {
        sum += arr[pos];
        pos += stride;
        if (pos >= bytes) {
            pos -= bytes;
        }
    }



    double best = 1e18;
    for (int r = 0; r  < REPEATS; r++) {
        double start = now_ns();
        for (long i = 0; i < ACCESSES; i++) {
            sum += arr[pos];
            pos += stride;
            if (pos >= bytes) {
                pos -= bytes;
            }
        }
        double end  = now_ns();

        double per_access = (end - start) / ACCESSES;
        if (per_access < best) {
            best = per_access;
        }
    }

    sink = sum;    /* use the result so the loop is not removed */

    free(arr);
    return best;
}

/* ---------- main ---------- */

/*
int main(void)
{
    srand(12345);

    save_sys_info("sys_cache.csv");

    FILE *out = fopen("results.csv", "w");
    if (out == NULL) {
        perror("results.csv");
        return 1;
    }



     one tidy table: experiment name, x value in bytes, time per access 
    fprintf(out, "experiment,x_bytes,ns_per_access\n");


    
    Experiment 1: 1 KB up to 64 MB, four sizes per doubling 
    for (size_t base = 1024; base <= 64UL * 1024 * 1024; base *= 2) {
        for (int k = 0; k < 4; k++) {
            size_t bytes = base + k * (base / 4);
            if (bytes > 64UL * 1024 * 1024) {
                break;
            }
            double t = time_chase(bytes);
            fprintf(out, "working_set,%zu,%.3f\n", bytes, t);
            fflush(out);
            printf("working set %zu bytes: %.3f ns\n", bytes, t);
        }
    }

    
     Experiment 2: 64 MB array, stride 1 byte up to 512 bytes 
    for (size_t stride = 1; stride <= 512; stride *= 2) {
        double t = time_stride(64UL * 1024 * 1024, stride);
        fprintf(out, "stride,%zu,%.3f\n", stride, t);
        fflush(out);
        printf("stride %zu bytes: %.3f ns\n", stride, t);
    }

    fclose(out);
    printf("Saved results.csv\n");

    return 0;
}*/
