#include <stdio.h>
#include "cpuInfo.h"

int main() {
    //demonstrate core_count function
    int core_count = get_count();
    printf("Amount of Cores\n%d\n----------", core_count);

    //demonstrate field_content function
    char field_content[100] = {0};
    parse_cpu_info("microcode", field_content);
    printf("\nContent of first field 'microcode'\n%s\n----------", field_content);

    //demonstrat model_name function
    char model_name[100] = {0};
    get_model_name(model_name);
    printf("\nFirst model name\n%s\n----------", model_name);

    //demonstrate core_stats function
    long core_stats[12] = {0};
    parse_stat(4, core_stats);
    printf("\nNumbers for core 4\n");
    for (int i = 0; i < 12;  i++) printf("%ld%c", core_stats[i], ' ');
    printf("\n----------");

    //demonstrate get_busy function
    printf("\nBusy ticks for core 4\n%ld\n----------",get_busy(4));

    //demonstrate get_idle function
    printf("\nIdle ticks for core 4\n%ld\n----------",get_idle(4));

    printf("\n");
    return 0;
}
