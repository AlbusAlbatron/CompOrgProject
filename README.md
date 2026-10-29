
gcc main.c cpuInfo.c cpuReport.c pciInfo.c pciReport.c mem_info.c mem_hierarchy.c id_lookup.c usb_report.c parallel_benchmark.c -pthread -o CompOrgProject

Our program reads data directly from these virtual files provided by the Linux kernel:

 Processor (CPU) Info:
  * Model Name & Core Count: Read from /proc/cpuinfo
  * Active Core Load Rates: /proc/stat
  * Hardware Cache Sizes: /sys/devices/system/cpu/cpu0/cache/

  Memory (RAM) Info: 
  * Total, Used, and Available Memory capacities: Parsed directly from /proc/meminfo

  PCI Devices Subsystem: 
  * Device attributes (bus, device, function, IDs, and classes): Found by traversing directories under /sys/bus/pci/devices/
  * Human-Readable Names: Mapped using the official pci.ids reference database file

  USB Devices Subsystem:
  * Device attributes (Vendor IDs and Product IDs): Found by traversing virtual directories under /sys/bus/usb/devices/
  * Human-Readable Names: Mapped using the official usb.ids reference database file
