#include "pciInfo.h"
#include <stdio.h>

int main(){

    //Creating an empty array to overwrite
    //Overwriting the array with the names of pci devices
    //Storing the amount of pci devices as pciCount
    //Sorting the list
    char *devices[60] = {NULL};
    int pciCount = createList(devices, 60);
    sortList(devices, pciCount);

    //Display the amount of pci devices
    printf("PCI Count\n");
    printf("%d\n", pciCount);

    //Print the array
    //for (int i = 0; i < pciCount; i++) printf("%s\n",devices[i]);


    printf("----------\nPCI Device\n");
    printf("%s\n", devices[6]);     //reference to see if I get the right substrings
    //Create an empty string to overwrite
    //Overwrites the string with the correct substring
    char bus[2];
    printf("\nBus\n");
    printf("%s\n", getBus(bus, devices, 6));

    char device[2];
    printf("\nDevice\n");
    printf("%s\n", getDevice(device, devices, 6));

    char function[1];
    printf("\nFunction\n");
    printf("%s\n", getFunction(function, devices, 6));


    //Same way to use as the above functions
    //Create your empty string to overwrite, then use getWhatever(thatString, devices, whichDevice)
    char vendorID[4];
    printf("----------\nVendor ID\n");
    printf("%s\n", getVendorID(vendorID, devices, 6));

    char deviceID[4];
    printf("\nDevice ID\n");
    printf("%s\n", getDeviceID(deviceID, devices, 6));

    char classCode[6];
    printf("\nClass Code\n");
    printf("%s\n", getClassCode(classCode, devices, 6));
}
