#include <stdio.h>
#include "cpuInfo.h"

int main() {
    //demonstrate core_count function
    //the function returns the value that you want, simply assign it to a variable
    int core_count = get_count();
    printf("Amount of Cores\n%d\n----------", core_count);

    //demonstrate field_content function
    //the function returns 1 if the field argument exists in the file, 0 otherwise
    //function overwrites a string to get output
    char field_content[100] = {0};
    parse_cpu_info("microcode", field_content);
    printf("\nContent of first field 'microcode'\n%s\n----------", field_content);

    //demonstrate model_name function
    //function always returns 0 so not useful
    //overwrites a string
    char model_name[100] = {0};
    get_model_name(model_name);
    printf("\nFirst model name\n%s\n----------", model_name);

    //demonstrate core_stats function
    //function overwrites an array of longs
    long core_stats[12] = {0};
    parse_stat(4, core_stats);
    printf("\nNumbers for core 4\n");
    for (int i = 0; i < 12;  i++) printf("%ld%c", core_stats[i], ' ');
    printf("\n----------");

    //demonstrate parse_stat return
    //function returns 1 if the core argument matches in the file
    //returns 0 otherwise
    printf("\nCore 4 does exist: %d", parse_stat(4, core_stats));
    printf("\nCore 14 does not exist: %d\n----------", parse_stat(14, core_stats));

    //demonstrate get_busy function
    //returns the value you want, simply assign the call to a variable
    printf("\nBusy ticks for core 4\n%ld\n----------",get_busy(4));

    //demonstrate get_idle function
    //returns the value you want, simply assign the call to a variable
    printf("\nIdle ticks for core 4\n%ld\n----------",get_idle(4));

    printf("\n");
    return 0;
}
