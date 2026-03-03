#include "lib.h"

#include <sys/ptrace.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s <process_name>\n", argv[0]);
        return 1;
    }

	DIR *dir;
	struct dirent *entry;

	dir = opendir("/proc");
	if (!dir) {
	    perror("opendir");
	    return 1;
	}
	MemRegion *tt;
	while ((entry = readdir(dir)) != NULL) {
	    int pid = atoi(entry->d_name);

	    if (pid <= 0) continue;
		
	    char path[256];
	    snprintf(path, sizeof(path), "/proc/%d/comm", pid);

	    char *str = get_file_data(path);
		
	    if (str) {
			if (strcmp(argv[1], str) == 0) {
				printf("PID %d: %s\n", pid, str);
				tt = get_memory_regions(pid, "wr");
			}
	        free(str);
	    }
	}

	if (tt)
		print_mem_region(tt);

    closedir(dir);
    return 0;
}