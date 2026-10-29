#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cpuReport.h"
#include "pciReport.h"
#include "mem_info.h"
#include "id_lookup.h"
#include "usb_report.h"
#include "parallel_benchmark.h"
#include "mem_hierarchy.h"

void display_mem_info();

int main(void)
{
    int choice;

    do {
        sleep(2);

        printf("\n--- Device Info Report ---\n");
        printf("1. CPU Report\n");
        printf("2. PCI Device Report\n");
        printf("3. Memory Report\n");
        printf("4. USB Device Report\n");
        printf("5. Parallel Performance Benchmark\n");
        printf("6. Memory Hierarchy Experiment\n");
        printf("7. Memory Hierarchy Experiment with Python graph\n");
        printf("0. Exit\n");

        printf("\nSelect an option: ");

        if (scanf("%d", &choice) != 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }

            printf("Please enter a number:\n");
            continue;
        }

        switch (choice) {
            case 1:
                printf("Running CPU Report\n");
                cpu_report();
                break;

            case 2:
                printf("PCI Device Report\n");
                pci_report();
                break;

            case 3:
                display_mem_info();
                break;

            case 4:
                printf("USB Device Report\n");
                {
                    // 1. Set up the temporary database here
                    IdLookUp* usb_db = malloc(sizeof(IdLookUp));
                    if (usb_db != NULL) {
                        usb_db->vendor_count = 0;
                        usb_db->device_count = 0;
                        usb_db->class_count = 0;
                    }

                    // 2. Open the CSV file here
                    FILE *csv_file = fopen("hardware_report.csv", "w");
                    if (csv_file != NULL) {
                        // Print the column headers
                        fprintf(csv_file, "Subsystem,Vendor ID,Product ID,Vendor Name,Product Name\n");
                    }

                    // 3. Pass BOTH the database and active file pointer
                    report_usb_devices(usb_db, csv_file);

                    // 4. Clean up everything safely before exiting the case
                    if (csv_file != NULL) {
                        fclose(csv_file);
                    }
                    if (usb_db != NULL) {
                        free(usb_db);
                    }
                }
                break;


            case 5:
                printf("---Parallel Benchmark---\n");
                run_parallel_performance_benchmark();
                printf("\n\n");
                break;
            case 6:
                printf("Memory Hierarchy\n");
                run_experiments();
                break;
            case 7:
                printf("Memory Hierarchy with Graph\n");
                run_experiments_and_plot();
                break;
            case 0:
                printf("Exiting\n");
                break;

            default:
                printf("Invalid option. Try again.\n");
                break;
        }

    } while (choice != 0);

    return 0;
}

void display_mem_info() {
    long total = get_total();
    long used = get_used();
    long available = get_available();
    double used_percent = get_used_percent();

    printf("\n=== Memory Report ===\n");

    printf("Total:     %ld KiB\n", total);
    printf("Used:      %ld KiB\n", used);
    printf("Available: %ld KiB\n", available);
    printf("Used Percent %0.2f%%\n\n", used_percent);
}

