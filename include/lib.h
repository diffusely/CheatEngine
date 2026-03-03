#ifndef LIB_H
#define LIB_H

#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "mem_region.h"

struct MemRegion;

// Get proc attributes
char *get_file_data(char *file_name);
int read_memory(int pid, unsigned long addr, void *buf, size_t len);

// Get proc memory info
MemRegion *get_memory_regions(int pid, char *mode);

#endif