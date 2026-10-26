#include "sysFile.h"

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Read all bytes from a file into a resizing buffer
 * Example file content: "MemTotal: 16384 kB\nMemFree: 2048 kB\n"
 */

static char *read_file(const char *path, size_t *length)
{
    if (path == NULL || length == NULL)
        return NULL;

    FILE *file = fopen(path, "rb");
    if (file == NULL)
        return NULL;

    size_t capacity = 4096;
    size_t used = 0;
    char *buffer = malloc(capacity + 1);
    if (buffer == NULL) {
        fclose(file);
        return NULL;
    }

    /*
    *really fun c infinite loop trick
    *no condition needed
    */
    for (;;) {
        if (used == capacity) {
            if (capacity > SIZE_MAX / 2 - 1) {
                free(buffer);
                fclose(file);
                return NULL;
            }

            size_t new_capacity = capacity * 2;
            char *new_buffer = realloc(buffer, new_capacity + 1);
            if (new_buffer == NULL) {
                free(buffer);
                fclose(file);
                return NULL;
            }

            buffer = new_buffer;
            capacity = new_capacity;
        }

        size_t bytes_read = fread(buffer + used, 1, capacity - used, file);
        used += bytes_read;

        if (bytes_read == 0) {
            if (ferror(file)) {
                free(buffer);
                fclose(file);
                return NULL;
            }
            break;
        }
    }

    fclose(file);
    buffer[used] = '\0';
    *length = used;
    return buffer;
}




/*
 * Build a path, optionally prefixing it with the HW_ROOT environment variable
 * Example: name="/proc/meminfo", HW_ROOT="/tmp/hw" -> "/tmp/hw/proc/meminfo"
 * 
 the leading slash is removed from name so the result stays under HW_ROOT
 */


char *sysfile_path(const char *name)
{
    if (name == NULL)
        return NULL;

    const char *root = getenv("HW_ROOT");
    if (root == NULL || root[0] == '\0')
        root = "";

    if (root[0] == '\0') {
        size_t length = strlen(name);
        char *path = malloc(length + 1);
        if (path != NULL)
            memcpy(path, name, length + 1);
        return path;
    }

    while (*name == '/' || *name == '\\')
        name++;

    size_t root_length = strlen(root);
    size_t name_length = strlen(name);


    /*feel free to refactor this bit if hard to read*/
    int needs_separator =
        root_length > 0 &&
        root[root_length - 1] != '/' &&
         root[root_length - 1] != '\\';


    if (root_length > SIZE_MAX - name_length - (size_t)needs_separator - 1)
        return NULL;

    size_t path_length = root_length + (size_t)needs_separator + name_length;
    char *path = malloc(path_length + 1);
    if (path == NULL)
        return NULL;


    memcpy(path, root, root_length);
    size_t offset = root_length;

    if (needs_separator)
        path[offset++] = '/';

    memcpy(path + offset, name, name_length);
    path[path_length] = '\0';
    return path;
}



/*
 * Reads a file and removes whitespace from its beginning and end
 * Example: "  0x8086\n" becomes "0x8086"
 * Trimming makes it easier to parse files that end with a newline
 */
char *sysfile_text(const char *path)
{
    size_t length;
    char *contents = read_file(path, &length);
    if (contents == NULL)
        return NULL;

    size_t start = 0;
    while (start < length && isspace((unsigned char)contents[start]))
        start++;

    while (length > start && isspace((unsigned char)contents[length - 1]))
        length--;

    size_t trimmed_length = length - start;
    if (start != 0)
        memmove(contents, contents + start, trimmed_length);

    contents[trimmed_length] = '\0';
    return contents;
}



/*
 * read a file into an allocated array of lines
 * Example: "MemTotal: 16384 kB\r\nMemFree: 2048 kB\r\n" becomes two strings
 * It can handles both LF and CRLF endings and removes the newline characters
 */

char **sysfile_lines(const char *path, size_t *line_count)
{
    if (line_count == NULL)
        return NULL;

    *line_count = 0;



    size_t length;
    char *contents = read_file(path, &length);
    if (contents == NULL)
        return NULL;

    size_t count = 0;
    for (size_t i = 0; i < length; i++) {
        if (contents[i] == '\n' || contents[i] == '\r') {
            count++;
            if (contents[i] == '\r' && i + 1 < length && contents[i + 1] == '\n')
                i++;
        }
    }


    if (length > 0 && contents[length - 1] != '\n' &&
        contents[length - 1] != '\r')
        count++;

    if (count == 0) {
        free(contents);
        return NULL;
    }

    if (count > SIZE_MAX / sizeof(char *)) {
        free(contents);
        return NULL;
    }

    char **lines = calloc(count, sizeof(char *));
    if (lines == NULL) {
        free(contents);
        return NULL;
    }

    size_t start = 0;
    size_t line_index = 0;

    for (size_t i = 0; i <= length; i++) {
        if (i == length || contents[i] == '\n' || contents[i] == '\r') {
            size_t line_length = i - start;
            lines[line_index] = malloc(line_length + 1);

            if (lines[line_index] == NULL) {
                sysfile_free_lines(lines, line_index);
                free(contents);
                return NULL;
            }

            memcpy(lines[line_index], contents + start, line_length);
            lines[line_index][line_length] = '\0';
            line_index++;

            if (i < length && contents[i] == '\r' &&
                i + 1 < length && contents[i + 1] == '\n')
                i++;

            start = i + 1;
        }
    }

    free(contents);
    *line_count = count;
    return lines;
}



/*
 * Frees every line and the array returned by sysfile_lines
 * Each line is separately allocated, so each must be freed before the array
 */

void sysfile_free_lines(char **lines, size_t line_count)
{
    if (lines == NULL)
        return;

    for (size_t i = 0; i < line_count; i++)
        free(lines[i]);

    free(lines);
}


/*
 * Parse a complete decimal or 0x-prefixed hexadecimal string
 * Example:  "42" becomes 42; "0x8086" becomes 32902
 * The checks reject overflow and trailing text such as "42xyz"
 */

static int64_t parse_number(const char *text)
{
    if (text == NULL)
        return -1;

    while (isspace((unsigned char)*text))
        text++;

    int base = 10;
    if (text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) {
        text += 2;
        base = 16;
    }

    errno = 0;
    char *end;
    long long value = strtoll(text, &end, base);

    if (text == end || errno == ERANGE)
        return -1;

    while (isspace((unsigned char)*end))
        end++;

    if (*end != '\0')
        return -1;

    return (int64_t)value;
}


/*
 * Read a file, trims it's contents, and parse them as a number

 * Example:  file content: "  0x8086\n" returns 32902
 * returns -1 if the file cannot be read or its contents are not a number.
 */

int64_t sysfile_number(const char *path)
{
    char *contents = sysfile_text(path);
    if (contents == NULL)
        return -1;

    int64_t value = parse_number(contents);
    free(contents);
    return value;
}