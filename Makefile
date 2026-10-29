CC ?= gcc
CFLAGS ?= -Wall -Wextra -pthread
LDFLAGS ?= -pthread

OBJECTS = main.o cpuInfo.o cpuReport.o id_lookup.o mem_info.o \
	parallel_benchmark.o pciInfo.o pciReport.o usb_report.o
DEPS = $(OBJECTS:.o=.d)

.PHONY: all clean

all: main

main: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -f main $(OBJECTS) $(DEPS)

-include $(DEPS)