#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_count() {
    int count = 0;

    FILE *file;
    file = fopen("/proc/cpuinfo", "r");
    char read_line[1000];
    while (fgets(read_line, sizeof read_line, file) != NULL){
        if ((read_line[0] == 'p') && (read_line[8] == 'r')) count++;
    };
    fclose(file);
    return count;
}

int parse_cpu_info(char request[], char* out_str) {
    FILE *file;
    file = fopen("/proc/cpuinfo", "r");

    //Format the input target field
    char stripped_request[100] = {' '};
    for (int i = 0, j = 0; i < strlen(request); i++) {
        if (request[i] == ' ') continue;
        if (request[i] == '\t') continue;
        stripped_request[j] = request[i];
        stripped_request[j+1] = ':';
        j++;
    }

    char read_line[1000];
    while (fgets(read_line, sizeof read_line, file) != NULL) {
        //Format the field of each line
        char stripped_field[100];
        for (int i = 0, j = 0; i < 100; i++) {
            if (read_line[i] == ' ') continue;
            if (read_line[i] == '\t') continue;
            stripped_field[j] = read_line[i];
            j++;
            if (read_line[i] == ':') {
                stripped_field[j] = '\0';
                break;
            }
        }
        if (!strcmp(stripped_field, stripped_request)) {
            int start_read_val = 0;
            for (int i = 0, j = 0; i < 100; i++) {
                if (start_read_val) {
                    out_str[j] = read_line[i];
                    j++;
                }
                else {
                    if (read_line[i] == ' ') continue;
                    if (read_line[i] == '\t') continue;
                    if (read_line[i] == ':') {
                        start_read_val = 1;
                        i++;
                    }
                }
            }
            out_str[strlen(out_str) - 1] = '\0';
            return 1;
        }
    }   

    fclose(file);
    return 0;
}

int get_model_name(char* out_str) {
    if (!parse_cpu_info("model name", out_str)) out_str = "unknown";
    return 0;
}

int parse_stat(int core, long out_arr[]) {
    FILE *file;
    file = fopen("/proc/stat", "r");

    char read_line[1000];
    while (fgets(read_line, sizeof read_line, file) != NULL) {
        if ((read_line[0] == 'c') && (read_line[1] == 'p')) {
            char line_num[10] = {' '};
            line_num[0] = read_line[3];
            line_num[1] = read_line[4];
            line_num[2] = '\0';
            long int_line_num = atol(line_num);
            printf("%ld\n",int_line_num);
            if (int_line_num == core) printf("\n%s", read_line);
        }
    }

    fclose(file);
}