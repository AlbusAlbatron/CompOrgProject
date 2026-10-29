#ifndef MEM_INFO_H
#define MEM_INFO_H

long get_total();
long get_available();
long get_used();
long get_mem_field(const char* name);
double get_used_percent();

#endif
