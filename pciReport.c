#include <stdio.h>
#include <stdlib.h>
#include "pciInfo.h"
#include "id_lookup.h"

int main() {
    IdLookUp* pci_db = malloc(sizeof(IdLookUp));
    init_pci_db(pci_db);

    char *devices[60] = {NULL};
    int pci_count = create_list(devices, 60);
    sort_list(devices, pci_count);

    printf("PCI Count: %d", pci_count);

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
        get_device_name(pci_db, strtol(device_id, NULL, 0), strtol(vendor_id, NULL, 0), result, sizeof(result));
        printf("Device: %s - ", device_id);
        printf("%s\n", result);
        get_class_name(pci_db, strtol(class_code, NULL, 0), result, sizeof(result));
        printf("Class: %s - ", class_code);
        printf("%s\n", result);
    }

    printf("\n");
}
