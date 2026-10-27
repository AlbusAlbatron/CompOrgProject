#ifndef USB_REPORT_H
#define USB_REPORT_H

#include "id_lookup.h"

// Maximum allowed length for constructed file system paths
#define MAX_PATH_LEN 512

 // Structure to hold raw and resolved human-readable data for a single connected USB device.
typedef struct {
    char vendor_id[16];   // Raw Vendor ID string from sysfs (e.g., "1d6b")
    char product_id[16];  // Raw Product ID string from sysfs (e.g., "0002")
    char vendor_name[128]; // Human-readable manufacturer name (e.g., "Linux Foundation")
    char product_name[128]; // Human-readable product name (e.g., "2.0 root hub")
} UsbDevice;

// Scans the Linux sys files to populate 'device_list' with raw IDs.
// Returns the total number of devices successfully gathered.

int gather_usb_devices(UsbDevice *device_list);

// Allocates device memory, triggers the hardware scan, converts IDs, resolves names via 'usb_db', and prints a clean layout report.

void report_usb_devices(IdLookUp *usb_db);

#endif // USB_REPORT_H

