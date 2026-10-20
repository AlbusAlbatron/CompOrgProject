#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define MAX_NAME_LEN 128
#define MAX_VENDORS 3000
#define MAX_DEVICES 3000
#define MAX_CLASSES 1000


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
int get_vendor_name(const IdLookUp *lookup_db, int vendor_id, char *out_name, int max_len);

int init_id_lookup(const char *filepath, IdLookUp *lookup_db) {
    if (filepath == NULL || lookup_db == NULL) {
        return -1;
    }

    lookup_db->vendor_count = 0;
    lookup_db-.device_count = 0;

    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        return -2;
    }

    char line[256];
    int current_vendor = -1;
    int baseClass = -1;
    int inClasses = 0;
    

    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == '\n' || line[0] == '#') {
            continue;
        }

        if (line[0] == 'C' && line[1] == ' ') {
            inClasses = 1;
            baseClass = strtol(&line[2], NULL, 16);
            if (lookup_db->class_count < MAX_CLASSES) {
                lookup_db->classes[lookup_db->class_count].key = ((uint32_t)baseClass << 8 | )
            }
        }
    }

    fclose(file);
    return 0;
}

int get_vendor_name(const IdLookUp *lookup_db, int vendor_id, char *out_name, int max_len) {
    if (lookup_db == NULL || out_name == NULL || max_len <= 0) {
        return -1;
    }

    for (int i = 0; i < lookup_db->vendor_count; i++) {
        if (lookup_db->vendors[i].id == vendor_id) {
            snprintf(out_name, max_len, "%s", lookup_db->vendor_name[i]);
            return 0;
        }
    }

    return -2;
}

static inline uint32_t make_device_key(uint16_t vendor, uint16_t device) {
    return ((uint32_t) vendor << 16) | device;
}
