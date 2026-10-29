#ifndef PCIINFO.H
#define PCIINFO.H

int create_list(char *devices[], int temp_max);
int sort_list(char *in_list[], int in_list_len);

char* get_bus(char bus[], char *pci_device);
char* get_device(char device[], char *pci_device);
char* get_function(char function[], char *pci_device);

char* get_vendor_id(char vendor_id[], char pci_device[]);
char* get_device_id(char device_id[], char pci_device[]);
char* get_class_code(char class_code[], char pci_device[]);

#endif
