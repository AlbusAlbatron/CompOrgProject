#ifndef MEM_HIERARCHY_H
#define MEM_HIERARCHY_H
 
#include <stddef.h>   /*for  size_t */

double time_chase(size_t bytes);
double time_stride(size_t bytes, size_t stride);
void save_sys_info(const char *filename);
 
#endif
