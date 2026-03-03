#include "lib.h"

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

	while ((entry = readdir(dir)) != NULL) {
	    int pid = atoi(entry->d_name);
	    if (pid <= 0) continue;

	    char path[256];
	    snprintf(path, sizeof(path), "/proc/%d/comm", pid);

	    char *str = get_comm_data(path);
	    if (str) {
			if (strcmp(argv[1], str) == 0) {
				printf("PID %d: %s\n", pid, str);
			}
	        free(str);
	    }
	}

    closedir(dir);
    return 0;
}