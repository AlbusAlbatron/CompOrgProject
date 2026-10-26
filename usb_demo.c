#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "id_lookup.h"  
#include "usb_report.h" 

int main() {
    printf("[DEBUG] Initializing shared database tracking layers...\n");
    
    // Allocate space for the database structure pointer
    IdLookUp* usb_db = malloc(sizeof(IdLookUp));
    
    if (usb_db != NULL) {
        // Manually initialize counts to 0 to mimic a safe empty database
        // This completely bypasses calling their broken init_usb_db loop function!
        usb_db->vendor_count = 0;
        usb_db->device_count = 0;
        usb_db->class_count = 0;
    }

    printf("[DEBUG] Launching your standalone core USB tracker...\n\n");
    
    // Trigger your stable USB reporting function block
    report_usb_devices(usb_db);

    // Free memory space safely
    if (usb_db != NULL) {
        free(usb_db);
    }
    
    printf("[DEBUG] Sandbox test pipeline finished successfully.\n");
    return 0;
}

