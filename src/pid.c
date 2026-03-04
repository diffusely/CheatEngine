#include "cheat_engine.h"

int get_pid_list(char *name, PidList *list)
{
	DIR *dir;
	struct dirent *entry;

	dir = opendir("/proc");
	if (!dir) {
	    perror("opendir");
	    return -1;
	}

	list->count = 0;
	while ((entry = readdir(dir)) != NULL && list->count < MAX_PIDS) {
	    int pid = atoi(entry->d_name);

	    if (pid <= 0) continue;
		
	    char path[256];
	    snprintf(path, sizeof(path), "/proc/%d/comm", pid);

	    char *str = get_file_data(path);
		
	    if (str) {
			if (strncmp(name, str, strlen(str)) == 0) {
				list->pids[list->count] = pid;
				strncpy(list->names[list->count], str, 255);
				list->count++;
				printf("[%d] PID %d: %s\n", list->count - 1, pid, str);
			}
	        free(str);
	    }
	}
	closedir(dir);
	return list->count;
}

int select_pid(PidList *list)
{
	if (list->count == 0) {
		printf("No processes found!\n");
		return -1;
	}
	
	if (list->count == 1) {
		printf("Auto-selected PID %d\n", list->pids[0]);
		return list->pids[0];
	}
	
	printf("Enter index [0-%d]: ", list->count - 1);
	int idx;
	scanf("%d", &idx);
	
	if (idx < 0 || idx >= list->count) {
		printf("Wrong PID index!\n");
		return -1;
	}
	
	printf("Selected PID %d (%s)\n", list->pids[idx], list->names[idx]);
	return list->pids[idx];
}