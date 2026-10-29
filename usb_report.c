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

// Math worker wrapper to spike CPU core loads
void* heavy_calculation_worker(void* arg) {
    (void)arg; 
    volatile unsigned long long i;
    for (i = 0; i < 500000000ULL; i++) {}
    return NULL;
}

// Spawns thread counts to analyze parallel execution latency intervals
double run_parallel_stress_test(int thread_count) {
    pthread_t *threads = malloc(sizeof(pthread_t) * thread_count);
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < thread_count; i++) {
        pthread_create(&threads[i], NULL, heavy_calculation_worker, NULL);
    }
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    free(threads);
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
}

// Scrapes virtual files under sysfs directory links
int gather_usb_devices(UsbDevice *device_list) {
    const char *root_path = "/sys/bus/usb/devices";
    int count = 0;
    DIR *dir = opendir(root_path);
    if (!dir) return 0; 

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL && count < 128) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        char path_v[MAX_PATH_LEN], path_p[MAX_PATH_LEN];
        snprintf(path_v, sizeof(path_v), "%s/%s/idVendor", root_path, entry->d_name);
        snprintf(path_p, sizeof(path_p), "%s/%s/idProduct", root_path, entry->d_name);

        FILE *fv = fopen(path_v, "r");
        FILE *fp = fopen(path_p, "r");

        if (fv && fp) {
            char raw_v[16] = {0}, raw_p[16] = {0};
            if (fscanf(fv, "%15s", raw_v) == 1 && fscanf(fp, "%15s", raw_p) == 1) {
                strncpy(device_list[count].vendor_id, raw_v, sizeof(device_list[count].vendor_id));
                strncpy(device_list[count].product_id, raw_p, sizeof(device_list[count].product_id));
                count++;
            }
        }
        if (fv) fclose(fv);
        if (fp) fclose(fp);
    }
    closedir(dir);
    return count; 
}

// Master logging handler: prints plain text to console AND logs table columns to file
void report_usb_devices(IdLookUp *usb_db, FILE *output_file) {
    UsbDevice *my_usb_array = malloc(sizeof(UsbDevice) * 128); 
    if (my_usb_array == NULL) return;
    
    // Task 1: Print text version straight to console
    printf("==================================================\n");
    printf("               USB DEVICE REPORT                  \n");
    printf("==================================================\n");

    int total_found = gather_usb_devices(my_usb_array);

    if (total_found == 0) {
        printf("No USB devices detected (Cloud Server / VM Environment).\n");
        
        // Task 2: Log compliance checkpoint into CSV spreadsheet file
        if (output_file != NULL) {
            fprintf(output_file, "USB,N/A,N/A,\"No USB devices detected (Cloud Server / VM Environment)\",\"N/A\"\n");
        }
    } else {
        printf("Detected %d USB Device(s):\n\n", total_found);
        for (int i = 0; i < total_found; i++) {
            int v_id_int = (int)strtol(my_usb_array[i].vendor_id, NULL, 16);
            int p_id_int = (int)strtol(my_usb_array[i].product_id, NULL, 16);

            if (usb_db != NULL) {
                get_vendor_name(usb_db, v_id_int, my_usb_array[i].vendor_name, sizeof(my_usb_array[i].vendor_name));
                get_device_name(usb_db, p_id_int, v_id_int, my_usb_array[i].product_name, sizeof(my_usb_array[i].product_name));
            } else {
                strncpy(my_usb_array[i].vendor_name, "Unknown Vendor (No DB)", 128);
                strncpy(my_usb_array[i].product_name, "Unknown Product (No DB)", 128);
            }

            printf("Device #%d: ID %s:%s | %s - %s\n", i + 1, 
                   my_usb_array[i].vendor_id, my_usb_array[i].product_id,
                   my_usb_array[i].vendor_name, my_usb_array[i].product_name);

            // Task 2: Log values to spreadsheet file grid
            if (output_file != NULL) {
                fprintf(output_file, "USB,0x%s,0x%s,\"%s\",\"%s\"\n", 
                        my_usb_array[i].vendor_id, my_usb_array[i].product_id,
                        my_usb_array[i].vendor_name, my_usb_array[i].product_name);
            }
        }
    }
    
    
    free(my_usb_array);
}




