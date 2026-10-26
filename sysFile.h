#ifndef SYSFILE_H
#define SYSFILE_H

#include <stddef.h>
#include <stdint.h>


char *sysfile_path(const char *name);
char *sysfile_text(const char *path);
char **sysfile_lines(const char *path, size_t *line_count);

int64_t sysfile_number(const char *path);

void sysfile_free_lines(char **lines, size_t line_count);

#endif