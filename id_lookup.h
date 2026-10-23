#ifndef ID_LOOKUP_H
#define ID_LOOKUP_H

#include <stdint.h>

#define MAX_NAME_LEN 256 
#define MAX_VENDORS 6000
#define MAX_DEVICES 6000
#define MAX_CLASSES 2000

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
} Vendor;

typedef struct {
    uint32_t key;
    char name[MAX_NAME_LEN];
} Device;

typedef struct {
    uint32_t key;
    char name[MAX_NAME_LEN];
} DeviceClass;

typedef struct {
    Vendor vendors[MAX_VENDORS];
    int vendor_count;

    Device devices[MAX_DEVICES];
    int device_count;
    
    DeviceClass classes[MAX_CLASSES];
    int class_count;
} IdLookUp;

int init_id_lookup(const char *filepath, IdLookUp *lookup_db);
int init_pci_db(IdLookUp *lookup_db);
int init_usb_db(IdLookUp *lookup_db);

int get_vendor_name(const IdLookUp *lookup_db, int vendor_id, char *out_name, int max_len);
int get_device_name(const IdLookUp *lookup_db, uint32_t device_id, uint32_t vendor_id, char *out_name, int max_len);
int get_class_name(const IdLookUp *lookup_db, uint32_t class_id, char *out_name, int max_len);

#endif
