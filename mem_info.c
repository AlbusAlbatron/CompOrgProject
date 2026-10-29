#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem_info.h"

//Total installed memory in KiB, or -1 if it cannot be read.
long get_total() {
    return get_mem_field("MemTotal");
}

//Memory that programs could use right now (free plus easily reclaimed), in KiB.
long get_available() {
    return get_mem_field("MemAvailable");
}

//Memeort in use in KiB: total minus available
long get_used() {
    long total = get_total();
    long available = get_available();
    return (total < 0 || available < 0) ? -1 : total - available;
}

double get_used_percent() {
    long total = get_total();
    long available = get_available();
    double used = (double)(total - available);
    
    return (total < 0 || available < 0) ? -1 : (used / total) * 100;
}

//The valie of KiB of a named line of /proc/meminfo
long get_mem_field(const char* name) {
    FILE *file = fopen("/proc/meminfo", "r");
    if (file == NULL) {
        return -2;
    }

    char line[256];
    size_t name_len = strlen(name);
    long result = -1;
    
    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == '\n') {
            continue;
        }

        if (strncmp(line, name, name_len) == 0 && line[name_len] == ':') {
            result = strtol(&line[name_len + 1], NULL, 10);

            break;
        }
    }

    fclose(file);
    return result;

}
