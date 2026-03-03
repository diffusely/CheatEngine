#include "lib.h"

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
    char path[64];
    snprintf(path, sizeof(path), "/proc/%d/mem", pid);
    
    int fd = open(path, O_RDONLY);
    if (fd < 0) return -1;
    
    if (lseek(fd, addr, SEEK_SET) == -1) {
        close(fd);
        return -1;
    }
    
    ssize_t n = read(fd, buf, len);
    close(fd);

    return (n == (ssize_t)len) ? 0 : -1;
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