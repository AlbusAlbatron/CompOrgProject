#ifndef USB_REPORT_H
#define USB_REPORT_H

#include "id_lookup.h"

#define MAX_PATH_LEN 512

typedef struct {
    char vendor_id[16];   
    char product_id[16];  
    char vendor_name[128];
    char product_name[128];
} UsbDevice;

int gather_usb_devices(UsbDevice *device_list);
void report_usb_devices(IdLookUp *usb_db);

#endif // USB_REPORT_H

