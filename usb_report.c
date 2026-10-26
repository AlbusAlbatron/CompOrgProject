#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_USB_DEVICES 128
#define MAX_PATH_LEN 512
#define MAX_LINE_LEN 256

// 1. Structure to store each USB device in your array
typedef struct {
    char vendor_id[8];   // Stores raw hex string (e.g. "1d6b")
    char product_id[8];  // Stores raw hex string (e.g. "0002")
    char vendor_name[128];
    char product_name[128];
} UsbDevice;

// 2. Lookup function that searches through the 'usb.ids' text database
void lookup_usb_names(UsbDevice *dev) {
    // Default fallbacks if no match is found in the text file
    strncpy(dev->vendor_name, "Unknown Vendor", sizeof(dev->vendor_name));
    strncpy(dev->product_name, "Unknown Product", sizeof(dev->product_name));

    FILE *f = fopen("usb.ids", "r");
    if (!f) {
        // If the database file is completely missing, just keep the fallbacks
        return; 
    }

    char line[MAX_LINE_LEN];
    int found_vendor = 0;

    // Scan usb.ids line by line
    while (fgets(line, sizeof(line), f)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n') continue;

        // Vendor lines have NO leading tab space
        if (line[0] != '\t') {
            char v_id[16], v_name[128];
            if (sscanf(line, "%15s %[^\n]", v_id, v_name) >= 2) {
                if (strcasecmp(v_id, dev->vendor_id) == 0) {
                    strncpy(dev->vendor_name, v_name, sizeof(dev->vendor_name));
                    found_vendor = 1; // Mark that we found the company
                    continue;
                }
            }
            found_vendor = 0; // Reset if we passed into a different vendor block
        }
        // Product lines have EXACTLY ONE leading tab space
        else if (line[0] == '\t' && line[1] != '\t' && found_vendor) {
            char p_id[16], p_name[128];
            if (sscanf(line, "%15s %[^\n]", p_id, p_name) >= 2) {
                if (strcasecmp(p_id, dev->product_id) == 0) {
                    strncpy(dev->product_name, p_name, sizeof(dev->product_name));
                    fclose(f);
                    return; // Both matches found, exit lookup early!
                }
            }
        }
    }
    fclose(f);
}

// 3. Scanner function that reads the /sys live Linux files
int gather_usb_devices(UsbDevice *device_list) {
    const char *root_path = "/sys/bus/usb/devices";
    int count = 0;

    DIR *dir = opendir(root_path);
    if (!dir) {
        return 0; // Return 0 if folder doesn't exist (Cloud/VM architecture fallback)
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL && count < MAX_USB_DEVICES) {
        // Ignore structural relative directory links
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char path_v[MAX_PATH_LEN], path_p[MAX_PATH_LEN];
        snprintf(path_v, sizeof(path_v), "%s/%s/idVendor", root_path, entry->d_name);
        snprintf(path_p, sizeof(path_p), "%s/%s/idProduct", root_path, entry->d_name);

        FILE *fv = fopen(path_v, "r");
        FILE *fp = fopen(path_p, "r");

        // If BOTH files open successfully, it means it's a valid hardware endpoint device
        if (fv && fp) {
            char raw_v[16] = {0}, raw_p[16] = {0};
            
            if (fscanf(fv, "%15s", raw_v) == 1 && fscanf(fp, "%15s", raw_p) == 1) {
                strncpy(device_list[count].vendor_id, raw_v, sizeof(device_list[count].vendor_id));
                strncpy(device_list[count].product_id, raw_p, sizeof(device_list[count].product_id));
                
                // Perform the database text lookup to resolve the names
                lookup_usb_names(&device_list[count]);
                count++;
            }
        }
        if (fv) fclose(fv);
        if (fp) fclose(fp);
    }
    closedir(dir);
    return count; // Returns the total number of USB items saved into the array
}

// 4. Main interface execution handler
int main() {
    // Allocation space for our array list
    UsbDevice my_usb_array[MAX_USB_DEVICES];
    
    printf("==================================================\n");
    printf("               USB DEVICE REPORT                  \n");
    printf("==================================================\n");

    int total_found = gather_usb_devices(my_usb_array);

    if (total_found == 0) {
        // Direct Core Requirement 4 Safeguard Fallback Rule
        printf("No USB devices detected (Cloud Server / VM Environment).\n");
    } else {
        printf("Detected %d USB Device(s):\n\n", total_found);
        for (int i = 0; i < total_found; i++) {
            printf("Device #%d:\n", i + 1);
            printf("  • Raw IDs: %s:%s\n", my_usb_array[i].vendor_id, my_usb_array[i].product_id);
            printf("  • Vendor:  %s\n", my_usb_array[i].vendor_name);
            printf("  • Product: %s\n\n", my_usb_array[i].product_name);
        }
    }
    printf("==================================================\n");
    return 0;
}
