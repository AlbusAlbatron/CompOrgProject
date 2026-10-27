#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include "usb_report.h"
#include "id_lookup.h"

// Loop to make the CPU core do heavy work
void* heavy_calculation_worker(void* arg) {
    (void)arg; 
    volatile unsigned long long i;
    
    // Count up to 500 million to generate high CPU usage
    for (i = 0; i < 500000000ULL; i++) {
        
        // Keep the CPU execution core busy
    }
    return NULL;
}

// Run threads and measure processing time 
double run_parallel_stress_test(int thread_count) {
    
    // Allocate a memory block to hold the thread array
    pthread_t *threads = malloc(sizeof(pthread_t) * thread_count);

    // Structure to hold the start and end timestamps
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Start all threads
    for (int i = 0; i < thread_count; i++) {
        pthread_create(&threads[i], NULL, heavy_calculation_worker, NULL);
    }

    // Join and synchronize execution tasks back into the primary thread
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    // Grab the exact current system time after everything completes
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Free up the thread array block from computer memory
    free(threads);

    // Calculate total elapsed seconds by checking seconds and nanoseconds
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;

    // Return total duration back to the caller
    return elapsed;
}

// Scans Linux system folders to gather a list of attached USB hardware
int gather_usb_devices(UsbDevice *device_list) {

    // Path where Linux keeps record of all connected USB hardware
    const char *root_path = "/sys/bus/usb/devices";
    int count = 0;

    // Open the system USB directory folder
    DIR *dir = opendir(root_path);

    // If the system path cannot be opened, exit with zero items
    if (!dir) return 0; 

    struct dirent *entry;

    // Look through files for up to 128 devices
    while ((entry = readdir(dir)) != NULL && count < 128) {

         // Skip current (.) and parent (..) folder shortcuts
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // String buffers to build the target file paths for vendor and product IDs
        char path_v[MAX_PATH_LEN], path_p[MAX_PATH_LEN];
        snprintf(path_v, sizeof(path_v), "%s/%s/idVendor", root_path, entry->d_name);
        snprintf(path_p, sizeof(path_p), "%s/%s/idProduct", root_path, entry->d_name);

        FILE *fv = fopen(path_v, "r");
        FILE *fp = fopen(path_p, "r");

        // Read the text IDs out of files if they exist
        if (fv && fp) {
            char raw_v[16] = {0}, raw_p[16] = {0};
            if (fscanf(fv, "%15s", raw_v) == 1 && fscanf(fp, "%15s", raw_p) == 1) {
                strncpy(device_list[count].vendor_id, raw_v, sizeof(device_list[count].vendor_id));
                strncpy(device_list[count].product_id, raw_p, sizeof(device_list[count].product_id));
                count++;
            }
        }
        // If Vendor and Product file handles are active, close them down
        if (fv) fclose(fv);
        if (fp) fclose(fp);
    }
    closedir(dir);
    return count; 
}

// Prints the complete USB list and thread test results
void report_usb_devices(IdLookUp *usb_db) {

    // Allocate space to hold details for up to 128 USB units
    UsbDevice *my_usb_array = malloc(sizeof(UsbDevice) * 128); 

    // If memory allocation crashes or fails, exit immediately
    if (my_usb_array == NULL) return;
    
    printf("==================================================\n");
    printf("               USB DEVICE REPORT                  \n");
    printf("==================================================\n");

    // Execute the device search function and get total count
    int total_found = gather_usb_devices(my_usb_array);

    // Handle state where no devices were located
    if (total_found == 0) {
        printf("No USB devices detected (Cloud Server / VM Environment).\n");
    } else {
        printf("Detected %d USB Device(s):\n\n", total_found);

        // Process and print out information for every discovered device
        for (int i = 0; i < total_found; i++) {

            // Convert hexadecimal string IDs to integers
            int v_id_int = (int)strtol(my_usb_array[i].vendor_id, NULL, 16);
            int p_id_int = (int)strtol(my_usb_array[i].product_id, NULL, 16);

            // Check if a lookup dictionary reference was passed in
            if (usb_db != NULL) {

                // Look up company/product names if database is open
                get_vendor_name(usb_db, v_id_int, my_usb_array[i].vendor_name, sizeof(my_usb_array[i].vendor_name));
                get_device_name(usb_db, p_id_int, v_id_int, my_usb_array[i].product_name, sizeof(my_usb_array[i].product_name));
            } else {

                // Handle case without database by using a fallback string
                strncpy(my_usb_array[i].vendor_name, "Unknown Vendor (No DB)", 128);
                strncpy(my_usb_array[i].product_name, "Unknown Product (No DB)", 128);
            }

            printf("Device #%d: ID %s:%s | %s - %s\n", i + 1, 
                   my_usb_array[i].vendor_id, my_usb_array[i].product_id,
                   my_usb_array[i].vendor_name, my_usb_array[i].product_name);
        }
    }
    
    // --- FINAL EXTENSION SUMMARY BLOCK ---
    printf("\n==================================================\n");
    printf("     LOAD & PARALLELISM ANALYSIS (AMDAHL'S LAW)    \n");
    printf("==================================================\n");
    printf("Running heavy parallel scaling worker tasks...\n\n");

    // Perform baseline performance check using 1, 2, and 4 threads
    double t1 = run_parallel_stress_test(1);
    double t2 = run_parallel_stress_test(2);
    double t4 = run_parallel_stress_test(4);

    // Print out times and speedups
    printf("Threads Run Profile Statistics:\n");
    printf("  • 1 Thread Runtime:  %.4f seconds (Baseline)\n", t1);
    printf("  • 2 Threads Runtime: %.4f seconds (Speedup: %.2fx)\n", t2, t1 / t2);
    printf("  • 4 Threads Runtime: %.4f seconds (Speedup: %.2fx)\n", t4, t1 / t4);

    // Output academic predictions based on Amdahl's hardware limits
    printf("\nTheoretical Comparison Metrics:\n");
    printf("  • Amdahl's Law Limit (Assuming P=0.90 Parallelizable Framework):\n");
    printf("    Maximum expected speedup for 2 Cores: 1.82x\n");
    printf("    Maximum expected speedup for 4 Cores: 3.08x\n");
    printf("  • Analytical Observation:\n");

    // Print notes explaining real world memory bottlenecks
    printf("    If measured, scalability runs slower than limits, processors\n");
    printf("    are bottlenecked by physical shared system memory caching limits.\n");
    printf("==================================================\n");

    // Clear list out of memory
    free(my_usb_array);
}



