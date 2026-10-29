#include <stdio.h>
#include <threads.h>
#include "cpuInfo.h"

int cpu_report() {

    int core_count = get_count();
    printf("Amount of Cores:\n%d\n----------", core_count);

    char model_name[100] = {0};
    get_model_name(model_name);
    printf("\nProcessor Model Name:\n%s\n----------", model_name);

    printf("\nProcessor Load Report\n");
    double total_busy;
    double total_idle;
    for (int i = 0; i < core_count; i++) {
        long core_stats[12] = {0};
        if (parse_stat(i, core_stats)){
            double busy = get_busy(i);
            double idle = get_idle(i);
            thrd_sleep(&(struct timespec){.tv_sec=1}, NULL);
            double busy2 = get_busy(i);
            double idle2 = get_idle(i);
            total_busy += busy + busy2;
            total_idle += idle + idle2;
            printf("\nBusy ticks for core %d:\t %f",i ,(busy + busy2/2));
            printf("\nIdle ticks for core %d:\t %f",i ,(idle + idle2/2));
            double load = ((busy + busy2 / 2)/((busy + busy2 / 2) + (idle + idle2 / 2)))*100;
            printf("\nLoad for core %d:\t %f%%\n",i ,load);
        }
    }
    double total_load = total_busy/(total_busy + total_idle) * 100;
    printf("\nAverage Load:\t %f%%\n",total_load);

    printf("\n");
    return 0;
}