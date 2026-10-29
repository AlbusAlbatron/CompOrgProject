#ifndef USB_REPORT_H
#define USB_REPORT_H

#include <stdio.h> // Necessary for defining the FILE streaming handle object
#include "id_lookup.h"

#define MAX_PATH_LEN 512

// Structure defining array storage constraints for tracked hardware modules
typedef struct {
    char vendor_id[16];   
    char product_id[16];  
    char vendor_name[128];
    char product_name[128];
} UsbDevice;

// Core processing declaration functions
int gather_usb_devices(UsbDevice *device_list);
void report_usb_devices(IdLookUp *usb_db, FILE *output_file);

#endif // USB_REPORT_H

