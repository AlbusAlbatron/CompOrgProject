#include <stdio.h>
#include "mem_info.h"

int main() {
    long mem_total = get_total();
    long mem_used = get_used();
    long mem_available = get_available();

    printf("Total: %ld\nUsed: %ld\nAvailable: %ld\n\n", mem_total, mem_used, mem_available);

    long swap_free_mem = get_mem_field("SwapFree");

    printf("SwapFree: %ld\n", swap_free_mem);

    return 0;
}
