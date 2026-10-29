#include <stdio.h>
#include <stdlib.h>
#include "id_lookup.h"  
#include "usb_report.h" 

int main() {
    printf("[DEBUG] Initializing shared database tracking layers...\n");
    
    // 1. Open a new file pointer target to save our data
    FILE *csv_file = fopen("hardware_report.csv", "w");
    if (csv_file == NULL) {
        printf("Error: Could not create output file layout.\n");
        return 1;
    }
    
    // 2. Print columns category headers at the top line of the CSV document
    fprintf(csv_file, "Subsystem,Vendor ID,Product ID,Vendor Name,Product Name\n");

    // Allocate space for the database structure pointer
    IdLookUp* usb_db = malloc(sizeof(IdLookUp));
    if (usb_db != NULL) {
        // Manually initialize database values to 0 to bypass their broken init loop function
        usb_db->vendor_count = 0;
        usb_db->device_count = 0;
        usb_db->class_count = 0;
    }

    printf("[DEBUG] Launching your standalone core USB tracker...\n\n");
    
    // 3. FIXED: Passing BOTH arguments (the database pointer AND the csv file tracker)
    report_usb_devices(usb_db, csv_file);

    // Free memory space and cleanly close our file stream
    if (usb_db != NULL) {
        free(usb_db);
    }
    fclose(csv_file); 
    
    printf("\n[DEBUG] Sandbox test pipeline finished successfully. 'hardware_report.csv' created!\n");
    return 0;
}


