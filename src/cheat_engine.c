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
		printf("[3] Write value\n");
		printf("[4] Refresh PIDs\n");
		printf("[5] Exit\n");
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
				memory_write(pid); 
				break;
			}
			case 4: {
				refresh(argv[1], &list); 
				break;
			}
			case 5: {
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