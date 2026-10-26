#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "usb_report.h"
#include "id_lookup.h"

// Define the missing path length limit locally
#define MAX_PATH_LEN 512

int gather_usb_devices(UsbDevice *device_list) {
    const char *root_path = "/sys/bus/usb/devices";
    int count = 0;

    DIR *dir = opendir(root_path);
    if (!dir) {
        return 0; 
    }

    struct dirent *entry;
    // FIX: Changed MAX_DEVICES to 128 to match the safe size allocated via malloc!
    while ((entry = readdir(dir)) != NULL && count < 128) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

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

void report_usb_devices(IdLookUp *usb_db) {
     // FIX: Dynamic heap allocation using malloc prevents the stack overflow crash
    UsbDevice *my_usb_array = malloc(sizeof(UsbDevice) * 128); 
    if (my_usb_array == NULL) {
        printf("Memory allocation error.\n");
        return;
    }
    
    printf("==================================================\n");
    printf("               USB DEVICE REPORT                  \n");
    printf("==================================================\n");

    int total_found = gather_usb_devices(my_usb_array);

    if (total_found == 0) {
        printf("No USB devices detected (Cloud Server / VM Environment).\n");
    } else {
        printf("Detected %d USB Device(s):\n\n", total_found);
        for (int i = 0; i < total_found; i++) {
            
            int v_id_int = (int)strtol(my_usb_array[i].vendor_id, NULL, 16);
            int p_id_int = (int)strtol(my_usb_array[i].product_id, NULL, 16);

            // Added pointer validation safety block to prevent crashes if the database initialization bugs out
            if (usb_db != NULL) {
                get_vendor_name(usb_db, v_id_int, my_usb_array[i].vendor_name, sizeof(my_usb_array[i].vendor_name));
                get_device_name(usb_db, p_id_int, v_id_int, my_usb_array[i].product_name, sizeof(my_usb_array[i].product_name));
            } else {
                strncpy(my_usb_array[i].vendor_name, "Unknown Vendor (No DB)", 128);
                strncpy(my_usb_array[i].product_name, "Unknown Product (No DB)", 128);
            }

            printf("Device #%d:\n", i + 1);
            printf("  • Raw IDs: %s:%s\n", my_usb_array[i].vendor_id, my_usb_array[i].product_id);
            printf("  • Vendor:  %s\n", my_usb_array[i].vendor_name);
            printf("  • Product: %s\n\n", my_usb_array[i].product_name);
        }
    }
    printf("==================================================\n");
    
    // Clean up our allocated heap space right before exiting
    free(my_usb_array);
}


