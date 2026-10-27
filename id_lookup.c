#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "id_lookup.h"

static inline uint32_t make_device_key(uint16_t vendor, uint16_t device);
static int is_hex(char* s, int from, int to);

int init_id_lookup(const char *filepath, IdLookUp *lookup_db) {
    if (filepath == NULL || lookup_db == NULL) {
        return -1;
    }

    lookup_db->vendor_count = 0;
    lookup_db->device_count = 0;
    lookup_db->class_count = 0;

    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        return -2;
    }

    char line[256];
    int current_vendor = -1;
    int base_class = -1;
    int in_classes = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == '\n' || line[0] == '#') {
            continue;
        }

        if (line[0] == 'C' && line[1] == ' ') {
            in_classes = 1;
            base_class = strtol(&line[2], NULL, 16);
            if (lookup_db->class_count < MAX_CLASSES) {
                lookup_db->classes[lookup_db->class_count].key = (uint32_t)base_class << 8;
                snprintf(lookup_db->classes[lookup_db->class_count].name, MAX_NAME_LEN, "%s", &line[6]);
                lookup_db->classes[lookup_db->class_count].name[strcspn(lookup_db->classes[lookup_db->class_count].name, "\n")] = '\0';
                lookup_db->class_count++;
            }
        }
        else if (in_classes && line[0] == '\t' && !(line[0] == '\t' && line[1] == '\t')) {
            if (base_class >= 0 && strlen(line) > 5 && is_hex(line, 1, 3)) {
                if (lookup_db->class_count < MAX_CLASSES) {
                    int sub_class = (int)strtol(&line[1], NULL, 16);
                    lookup_db->classes[lookup_db->class_count].key = (uint32_t)((base_class << 8) | sub_class);
                    snprintf(lookup_db->classes[lookup_db->class_count].name, MAX_NAME_LEN, "%s", &line[5]);
                    lookup_db->classes[lookup_db->class_count].name[strcspn(lookup_db->classes[lookup_db->class_count].name, "\n")] = '\0';
                    lookup_db->class_count++;
                }
            }
        }
        else if (isupper(line[0]) && !(line[0] == 'C' && line[1] == ' ')) {
                base_class = -1;
        }
        else if (is_hex(line, 0, 4) && strlen(line) > 6) {
            if (lookup_db->vendor_count < MAX_VENDORS) {
                lookup_db->vendors[lookup_db->vendor_count].id = (int)strtol(line, NULL, 16);
                snprintf(lookup_db->vendors[lookup_db->vendor_count].name, MAX_NAME_LEN, "%s", &line[6]);
                lookup_db->vendors[lookup_db->vendor_count].name[strcspn(lookup_db->vendors[lookup_db->vendor_count].name, "\n")] = '\0';
                lookup_db->vendor_count++;
                current_vendor++;
            }
        }
        else if (line[0] == '\t' && !(line[0] == '\t' && line[1] == '\t') && current_vendor >= 0 && is_hex(line, 1, 5) && strlen(line) > 7) {
            if (lookup_db->device_count < MAX_DEVICES) {
                int device = (int)strtol(&line[1], NULL, 16);
                lookup_db->devices[lookup_db->device_count].key = make_device_key(lookup_db->vendors[current_vendor].id, device);
                snprintf(lookup_db->devices[lookup_db->device_count].name, MAX_NAME_LEN, "%s" , &line[7]);
                lookup_db->devices[lookup_db->device_count].name[strcspn(lookup_db->devices[lookup_db->device_count].name, "\n")] = '\0';
                lookup_db->device_count++;
            }
    }
    }

    fclose(file);
    return 0;
}

int init_pci_db(IdLookUp* lookup_db) {
    if (lookup_db == NULL) {
        return -1;
    }

    const char *paths[] = {"pci.ids", "/usr/share/misc/pci.ids", "/usr/share/hwdata/pci.ids"};

    int path_count = sizeof(paths) / sizeof(paths[0]);

    for (int i = 0; i < path_count; i++) {
        int result = init_id_lookup(paths[i], lookup_db);
        
        if (result == 0) {
            return 0;
        }
    }

    return -2;
}

int init_usb_db(IdLookUp *lookup_db) {
    if (lookup_db == NULL) {
        return -1;
    }

    const char *paths[] = {"usb.ids", "/usr/share/misc/usb.ids", "/usr/share/hwdata/usb.ids",
                    "/var/lib/usbutils/usb.ids"};

    int path_count = sizeof(paths) / sizeof(paths[0]);

    for (int i = 0; i < path_count; i++) {
        int result = init_id_lookup(paths[i], lookup_db);

        if (result == 0) {
            return 0;
        }
    }

    return -2;
}

int get_vendor_name(const IdLookUp *lookup_db, int vendor_id, char *out_name, int max_len) {
    if (lookup_db == NULL || out_name == NULL || max_len <= 0) {
        return -1;
    }

    for (int i = 0; i < lookup_db->vendor_count; i++) {
        if (lookup_db->vendors[i].id == vendor_id) {
            snprintf(out_name, max_len, "%s", lookup_db->vendors[i].name);
            return 0;
        }
    }

    return -2;
}

int get_device_name(const IdLookUp *lookup_db, uint32_t device_id, uint32_t vendor_id, char* out_name, int max_len) {
    if (lookup_db == NULL || out_name == NULL || max_len <= 0) {
        return -1;
    }

    for (int i = 0; i < lookup_db->device_count; i++) {
        if (lookup_db->devices[i].key == make_device_key(vendor_id, device_id)){
            snprintf(out_name, max_len, "%s", lookup_db->devices[i].name);
            return 0;
        }
    }

    return -2;
}

int get_class_name(const IdLookUp *lookup_db, uint32_t class_id, char* out_name, int max_len) {
    if (lookup_db == NULL || out_name == NULL || max_len <= 0) {
        return -1;
    }

    uint32_t sub_key = class_id >> 8;
    uint32_t base_key = (class_id >> 16) << 8;
    int base_index = -1;

    for (int i = 0; i < lookup_db->class_count; i++) {
        if (lookup_db->classes[i].key == sub_key) {
            snprintf(out_name, max_len, "%s", lookup_db->classes[i].name);
            return 0;
        }

        else if (lookup_db->classes[i].key == base_key) {
            base_index = i;
        }
    }

    if (base_index != -1) {
        snprintf(out_name, max_len, "%s", lookup_db->classes[base_index].name);
        return 0;
    }

    return -2;
}

static inline uint32_t make_device_key(uint16_t vendor, uint16_t device) {
    return ((uint32_t) vendor << 16) | device;
}

static int is_hex(char* s, int from, int to) {
    if (strlen(s) < to) {
        return 0;
    }

    for (int i = from; i < to; i++) {
        if (!isxdigit(s[i])) {
            return 0;
        }
    }

    return 1;
}
