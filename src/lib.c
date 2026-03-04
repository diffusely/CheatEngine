#define _GNU_SOURCE
#include "lib.h"
#include <sys/uio.h>
#include <errno.h>

char *get_file_data(char *file_name)
{
    FILE *file = fopen(file_name, "r");
    if (!file) {
        return NULL;
    }

    char buffer[256];
    if (!fgets(buffer, sizeof(buffer), file)) {
        fclose(file);
        return NULL;
    }

    fclose(file);

    char *newline = strchr(buffer, '\n');
    if (newline) *newline = '\0';

    char *result = malloc(strlen(buffer) + 1);
    if (!result) {
        perror("malloc");
        return NULL;
    }
    strcpy(result, buffer);
    return result;
}

int read_memory(int pid, unsigned long addr, void *buf, size_t len)
{
    struct iovec local[1];
    struct iovec remote[1];
    
    local[0].iov_base = buf;
    local[0].iov_len = len;
    remote[0].iov_base = (void *)addr;
    remote[0].iov_len = len;
    
    ssize_t n = process_vm_readv(pid, local, 1, remote, 1, 0);
    return (n == (ssize_t)len) ? 0 : -1;
}

int write_memory(int pid, unsigned long addr, void *buf, size_t len)
{
    struct iovec local[1];
    struct iovec remote[1];
    
    local[0].iov_base = buf;
    local[0].iov_len = len;
    remote[0].iov_base = (void *)addr;
    remote[0].iov_len = len;
    
    ssize_t n = process_vm_writev(pid, local, 1, remote, 1, 0);
    return (n == (ssize_t)len) ? 0 : -1;
}

void scan_memory(int pid, MemRegion *regions, int value)
{
    printf("Scanning for value: %d\n", value);
    int count = 0;
    int region_num = 0;
    
    while (regions) {
        unsigned long size = regions->end - regions->start;
        unsigned char *buf = malloc(size);
        
        if (!buf) {
            regions = regions->next;
            region_num++;
            continue;
        }
        
        int res = read_memory(pid, regions->start, buf, size);
        if (res != 0) {
            free(buf);
            regions = regions->next;
            region_num++;
            continue;
        }
        
        for (unsigned long i = 0; i <= size - sizeof(int); i++) {
            int *ptr = (int *)(buf + i);
            if (*ptr == value) {
                printf("Found: 0x%lx = %d\n", regions->start + i, value);
                count++;
            }
        }
        free(buf);
        regions = regions->next;
        region_num++;
    }
    printf("Total matches: %d\n", count);
}

MemRegion *get_memory_regions(int pid, char *mode)
{
	MemRegion *res = NULL;
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    
    FILE *f = fopen(path, "r");
    if (!f) return NULL;
    
    char line[512];
    while (fgets(line, sizeof(line), f)) {
        unsigned long start, end;
        char perms[5];
		
        sscanf(line, "%lx-%lx %4s", &start, &end, perms);
        
        // Check permissions based on mode
        int match = 1;
        for (int i = 0; mode[i] && i < 4; i++) {
            if (mode[i] == 'r' && perms[0] != 'r') match = 0;
            if (mode[i] == 'w' && perms[1] != 'w') match = 0;
            if (mode[i] == 'x' && perms[2] != 'x') match = 0;
        }
        
        if (match) {
			MemRegion *new = create_node(start, end, perms);
			push_back(&res, new);
        }
    }
    fclose(f);
	return res;
}