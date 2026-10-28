#include "pciInfo.h"
#include <stdio.h>

int main(){

    //Creating an empty array to overwrite
    //Overwriting the array with the names of pci devices
    //Storing the amount of pci devices as pciCount
    //Sorting the list
    char *devices[60] = {NULL};
    int pci_count = create_list(devices, 60);
    sort_list(devices, pci_count);

    //Display the amount of pci devices
    printf("PCI Count\n");
    printf("%d\n", pci_count);

    //Print the array
    for (int i = 0; i < pci_count; i++) printf("%s\n",devices[i]);


    printf("----------\nPCI Device\n");
    printf("%s\n", devices[6]);     //reference to see if I get the right substrings
    //Create an empty string to overwrite
    //Overwrites the string with the correct substring
    char bus[2];
    printf("\nBus\n");
    printf("%s\n", get_bus(bus, devices[6]));

    char device[2];
    printf("\nDevice\n");
    printf("%s\n", get_device(device, devices[6]));

    char function[1];
    printf("\nFunction\n");
    printf("%s\n", get_function(function, devices[10]));


    //Same way to use as the above functions
    //Create your empty string to overwrite, then use getWhatever(thatString, devices, whichDevice)
    char vendor_id[10];
    printf("----------\nVendor ID\n");
    printf("%s\n", get_vendor_id(vendor_id, devices[6]));

    char device_id[10];
    printf("\nDevice ID\n");
    printf("%s\n", get_device_id(device_id, devices[6]));

    char class_code[10];
    printf("\nClass Code\n");
    printf("%s\n", get_class_code(class_code, devices[6]));
}
