#ifndef CPUINFO.H
#define CPUINFO.H

int get_count();
int parse_cpu_info(char name[], char* out_str);
int get_model_name(char* out_str);
int parse_stat(int core, long out_arr[]);
long get_busy(int core);
long get_idle(int core);

#endif
