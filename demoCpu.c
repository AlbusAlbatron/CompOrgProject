#include <stdio.h>
#include "cpuInfo.h"

int main() {
    int core_count = get_count();
    printf("Amount of Cores\n%d\n----------", core_count);

    char field_content[100] = {0};
    parse_cpu_info("microcode", field_content);
    printf("\nContent of first field 'microcode'\n%s\n----------", field_content);

    char model_name[100] = {0};
    get_model_name(model_name);
    printf("\nFirst model name\n%s\n----------", model_name);

    long core_stats[12] = {0};
    parse_stat(4, core_stats);
    for (int i = 0; i < 12;  i++) printf("%ld%c", core_stats[i], ' ');
    printf("\n");
    //parse_stat(10, core_stats);

    printf("\n");
    return 0;
}
