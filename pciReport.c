#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pciInfo.h"
#include "id_lookup.h"
#include "pciReport.h"

int main() {
    IdLookUp* pci_db = malloc(sizeof(IdLookUp));
    init_pci_db(pci_db);

    char *devices[60] = {NULL};
    int pci_count = create_list(devices, 60);
    sort_list(devices, pci_count);

    printf("PCI Count: %d", pci_count);

    char *vendor_list[pci_count];
    char *unique_vendor_list[60] = {NULL};
    char *class_list[pci_count];
    char *unique_class_list[60] = {NULL};

    printf("\n----------\nPCI Device List\n");
    for (int i = 0; i < pci_count; i++) {
        char bus[2] = {0};
        char device[2] = {0};
        char function[2] = {0};
        
        printf("\nPCI Device %d:\t",i);
        printf("%s:", get_bus(bus, devices[i]));
        printf("%s.", get_device(device, devices[i]));
        printf("%s", get_function(function, devices[i]));
        printf("\n");

        char vendor_id[10];
        get_vendor_id(vendor_id, devices[i]);
        char device_id[10];
        get_device_id(device_id, devices[i]);
        char class_code[12];
        get_class_code(class_code, devices[i]);
        char result[MAX_NAME_LEN];
        get_vendor_name(pci_db, strtol(vendor_id, NULL, 0), result, sizeof(result));
        printf("Vendor: %s - ", vendor_id);
        printf("%s\n", result);
        vendor_list[i] = strdup(result);
        get_device_name(pci_db, strtol(device_id, NULL, 0), strtol(vendor_id, NULL, 0), result, sizeof(result));
        printf("Device: %s - ", device_id);
        printf("%s\n", result);
        get_class_name(pci_db, strtol(class_code, NULL, 0), result, sizeof(result));
        printf("Class: %s - ", class_code);
        printf("%s\n", result);
        class_list[i] = strdup(result);
    }

    printf("\n----------\nDevice count by Vendor\n");
    int unique_devices = 0;
    int vendors_counts[60] = {0};
    for (int i = 0; i < pci_count; i++) {
        int check_null = 1;
        for (int j = 0; j < unique_devices; j++) {
            if (!strcmp(unique_vendor_list[j], vendor_list[i])) {
                vendors_counts[j] ++;
                check_null = 0;
                break;
            } 
        }

        if (unique_vendor_list[unique_devices] == NULL && check_null) {
            unique_vendor_list[unique_devices] = strdup(vendor_list[i]);
            vendors_counts[unique_devices] ++;
            unique_devices ++;
        }
    }

    for (int i = 0; i < pci_count; i++) {
        if (unique_vendor_list[i] != NULL) printf("\n%s : %d", unique_vendor_list[i], vendors_counts[i]);
    }

    printf("\n\n----------\nDevice count by Class\n");
    int unique_classes = 0;
    int class_counts[60] = {0};
    for (int i = 0; i < pci_count; i++) {
        int check_null = 1;
        for (int j = 0; j < unique_classes; j++) {
            if (!strcmp(unique_class_list[j], class_list[i])) {
                class_counts[j] ++;
                check_null = 0;
                break;
            } 
        }

        if (unique_class_list[unique_classes] == NULL && check_null) {
            unique_class_list[unique_classes] = strdup(class_list[i]);
            class_counts[unique_classes] ++;
            unique_classes ++;
        }
    }

    for (int i = 0; i < pci_count; i++) {
        if (unique_class_list[i] != NULL) printf("\n%s : %d", unique_class_list[i], class_counts[i]);
    }

    printf("\n\n");
}
