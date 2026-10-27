#include "pciInfo.h"
#include <stdio.h>

int main() {

    char *devices[60] = {NULL};
    int pci_count = create_list(devices, 60);
    sort_list(devices, pci_count);

    printf("PCI Count: %d\n", pci_count);

    return 0;
}