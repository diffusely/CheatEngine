#include "cheat_engine.h"

static void choose_pid(MemRegion **mem, PidList *list, int *pid)
{
	int new_pid = select_pid(list);
	if (new_pid != -1) {
		*pid = new_pid;
		if (*mem) free_mem(*mem);
		*mem = get_memory_regions(*pid, (char *)"rw");
		if (!*mem) {
			printf("Failed to get memory regions!\n");
			*pid = -1;
		} else {
			printf("Ready to scan PID %d\n", *pid);
		}
	}
}

static void memory_scan(MemRegion *mem, int pid)
{
	if (pid == -1) {
		printf("Select PID first!\n");
		return;
	}
	printf("Enter value: ");
	int val;
	scanf("%d", &val);
	scan_memory(pid, mem, val);
}

static void memory_write(int pid)
{
	if (pid == -1) {
		printf("Select PID first!\n");
		return;
	}
	printf("Enter address (0x...): ");
	unsigned long addr;
	scanf("%lx", &addr);
	printf("Enter new value: ");
	int new_val;
	scanf("%d", &new_val);
	if (write_memory(pid, addr, &new_val, sizeof(int)) == 0)
		printf("Done!\n");
	else
		printf("Failed to write\n");
}

static void refresh(char *name, PidList *list)
{
	printf("Refreshing...\n");
	int count = get_pid_list(name, list);
	if (count == 0)
		printf("No processes found!\n");
}

// TO DO
static unsigned long saved_addrs[MAX_SAVED];
static int saved_count = 0;

static void scan_diff(MemRegion *mem, int pid)
{
	if (pid == -1) {
		printf("Select PID first!\n");
		return;
	}
	
	printf("Enter value: ");
	int val;
	scanf("%d", &val);
	
	if (saved_count == 0) {
		printf("First scan for %d...\n", val);
		while (mem && saved_count < MAX_SAVED) {
			unsigned long size = mem->end - mem->start;
			unsigned char *buf = malloc(size);
			if (buf && read_memory(pid, mem->start, buf, size) == 0) {
				for (unsigned long i = 0; i <= size - sizeof(int) && saved_count < MAX_SAVED; i++) {
					int *ptr = (int *)(buf + i);
					if (*ptr == val) {
						saved_addrs[saved_count++] = mem->start + i;
					}
				}
			}
			if (buf) free(buf);
			mem = mem->next;
		}
		printf("Saved %d addresses\n", saved_count);
	} else {
		printf("Comparing %d addresses for value %d...\n", saved_count, val);
		int new_count = 0;
		for (int i = 0; i < saved_count; i++) {
			int cur;
			if (read_memory(pid, saved_addrs[i], &cur, sizeof(int)) == 0) {
				if (cur == val) {
					printf("MATCH: 0x%lx = %d\n", saved_addrs[i], cur);
					saved_addrs[new_count++] = saved_addrs[i];
				}
			}
		}
		saved_count = new_count;
		printf("Remaining: %d\n", saved_count);
		
		if (saved_count == 0) {
			printf("No matches! Resetting...\n");
		}
	}
}

static void reset_diff(void)
{
	saved_count = 0;
	printf("Diff reset!\n");
}

int run(char **argv)
{
	PidList list;
	int count = get_pid_list(argv[1], &list); 
	if (count == -1)
		return 1;
	if (count == 0) {
		printf("Process '%s' not found!\n", argv[1]);
		return 1;
	}

	int running = 1;

	MemRegion *mem = NULL;
	int pid = -1;
	while (running) {
		printf("\n[1] Select PID\n");
		printf("[2] Scan value\n");
		printf("[3] Diff scan (%d saved)\n", saved_count);
		printf("[4] Reset diff\n");
		printf("[5] Write value\n");
		printf("[6] Refresh PIDs\n");
		printf("[0] Exit\n");
		printf("> ");

		int choice;
		scanf("%d", &choice);

		switch (choice) {
			case 1: {
				choose_pid(&mem, &list, &pid); 
				break;
			}
			case 2: {
				memory_scan(mem, pid); 
				break;
			}
			case 3: {
				scan_diff(mem, pid);
				break;
			}
			case 4: {
				reset_diff();
				break;
			}
			case 5: {
				memory_write(pid); 
				break;
			}
			case 6: {
				refresh(argv[1], &list); 
				break;
			}
			case 0: {
				running = 0; 
				break;
			}
			default: {
				printf("Invalid option\n");
			}
		}
	}

	if (mem)
		free_mem(mem);
	return 0;
}