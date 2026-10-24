#include <stdio.h>
#include <stdlib.h>
#include "id_lookup.h"

int main() {
    IdLookUp* pci_db = malloc(sizeof(IdLookUp));
    IdLookUp* usb_db = malloc(sizeof(IdLookUp));

    init_pci_db(pci_db);
    init_usb_db(usb_db);

    char result[MAX_NAME_LEN];

    get_vendor_name(pci_db, 0x8086, result, sizeof(result));
    printf("Vendor 0x8086: %s\n", result);

    get_device_name(pci_db, 0x1237, 0x8086, result, sizeof(result));
    printf("Device 0x8086 0x1237: %s\n", result);

    get_class_name(pci_db, 0x030000, result, sizeof(result));
    printf("Class  0x030000: %s\n", result);
    
    free(pci_db);
    free(usb_db);

    return 0;
}
