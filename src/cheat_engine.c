#include "cheat_engine.h"

typedef enum {
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_DOUBLE
} ValueType;

static ValueType current_type = TYPE_INT;

static const char *type_names[] = {"int", "float", "double"};
static const size_t type_sizes[] = {sizeof(int), sizeof(float), sizeof(double)};

typedef union {
	int i;
	float f;
	double d;
} Value;

static void clear_input(void)
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

static int get_int(const char *prompt, int *out)
{
	printf("%s", prompt);
	if (scanf("%d", out) != 1) {
		clear_input();
		printf("Error: Invalid integer!\n");
		return -1;
	}
	return 0;
}

static int get_hex(const char *prompt, unsigned long *out)
{
	printf("%s", prompt);
	if (scanf("%lx", out) != 1) {
		clear_input();
		printf("Error: Invalid hex address!\n");
		return -1;
	}
	return 0;
}

static int get_value(const char *prompt, Value *v, ValueType type)
{
	printf("%s (%s): ", prompt, type_names[type]);
	int result = 0;
	
	switch (type) {
		case TYPE_INT:
			result = scanf("%d", &v->i);
			break;
		case TYPE_FLOAT:
			result = scanf("%f", &v->f);
			break;
		case TYPE_DOUBLE:
			result = scanf("%lf", &v->d);
			break;
	}
	
	if (result != 1) {
		clear_input();
		printf("Error: Invalid %s value!\n", type_names[type]);
		return -1;
	}
	return 0;
}

// ============ VALUE FUNCTIONS ============

static void print_value(Value v, ValueType type)
{
	switch (type) {
		case TYPE_INT:    printf("%d", v.i); break;
		case TYPE_FLOAT:  printf("%.2f", v.f); break;
		case TYPE_DOUBLE: printf("%.4lf", v.d); break;
	}
}

static int compare_value(void *mem_ptr, Value v, ValueType type)
{
	switch (type) {
		case TYPE_INT:    return *(int *)mem_ptr == v.i;
		case TYPE_FLOAT:  return *(float *)mem_ptr == v.f;
		case TYPE_DOUBLE: return *(double *)mem_ptr == v.d;
	}
	return 0;
}

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
	
	Value val;
	if (get_value("Enter value", &val, current_type) != 0)
		return;
	
	printf("Scanning for ");
	print_value(val, current_type);
	printf(" (%s)...\n", type_names[current_type]);
	
	int count = 0;
	size_t val_size = type_sizes[current_type];
	
	while (mem) {
		unsigned long size = mem->end - mem->start;
		unsigned char *buf = malloc(size);
		if (buf && read_memory(pid, mem->start, buf, size) == 0) {
			for (unsigned long i = 0; i <= size - val_size; i++) {
				if (compare_value(buf + i, val, current_type)) {
					printf("Found: 0x%lx = ", mem->start + i);
					print_value(val, current_type);
					printf("\n");
					count++;
				}
			}
		}
		if (buf) free(buf);
		mem = mem->next;
	}
	printf("Total: %d matches\n", count);
}

static void memory_write(int pid)
{
	if (pid == -1) {
		printf("Select PID first!\n");
		return;
	}
	
	unsigned long addr;
	if (get_hex("Enter address (0x...): ", &addr) != 0)
		return;
	
	Value new_val;
	if (get_value("Enter new value", &new_val, current_type) != 0)
		return;
	
	if (write_memory(pid, addr, &new_val, type_sizes[current_type]) == 0)
		printf("Done!\n");
	else
		printf("Failed to write\n");
}

static void change_type(void)
{
	printf("\nSelect type:\n");
	printf("[1] int (4 bytes)\n");
	printf("[2] float (4 bytes)\n");
	printf("[3] double (8 bytes)\n");
	
	int choice;
	if (get_int("> ", &choice) != 0)
		return;
	
	switch (choice) {
		case 1: current_type = TYPE_INT; break;
		case 2: current_type = TYPE_FLOAT; break;
		case 3: current_type = TYPE_DOUBLE; break;
		default: printf("Invalid choice\n"); return;
	}
	printf("Type set to: %s\n", type_names[current_type]);
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
	
	Value val;
	if (get_value("Enter value", &val, current_type) != 0)
		return;
	
	size_t val_size = type_sizes[current_type];
	
	if (saved_count == 0) {
		printf("First scan for ");
		print_value(val, current_type);
		printf("...\n");
		
		while (mem && saved_count < MAX_SAVED) {
			unsigned long size = mem->end - mem->start;
			unsigned char *buf = malloc(size);
			if (buf && read_memory(pid, mem->start, buf, size) == 0) {
				for (unsigned long i = 0; i <= size - val_size && saved_count < MAX_SAVED; i++) {
					if (compare_value(buf + i, val, current_type)) {
						saved_addrs[saved_count++] = mem->start + i;
					}
				}
			}
			if (buf) free(buf);
			mem = mem->next;
		}
		printf("Saved %d addresses\n", saved_count);
	} else {
		printf("Comparing %d addresses...\n", saved_count);
		int new_count = 0;
		for (int i = 0; i < saved_count; i++) {
			Value cur;
			if (read_memory(pid, saved_addrs[i], &cur, val_size) == 0) {
				if (compare_value(&cur, val, current_type)) {
					printf("MATCH: 0x%lx = ", saved_addrs[i]);
					print_value(cur, current_type);
					printf("\n");
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
		printf("\n=== Type: %s ===\n", type_names[current_type]);
		printf("[1] Select PID\n");
		printf("[2] Scan value\n");
		printf("[3] Diff scan (%d saved)\n", saved_count);
		printf("[4] Reset diff\n");
		printf("[5] Write value\n");
		printf("[6] Refresh PIDs\n");
		printf("[7] Change type\n");
		printf("[0] Exit\n");

		int choice;
		if (get_int("> ", &choice) != 0)
			continue;

		switch (choice) {
			case 1: choose_pid(&mem, &list, &pid); break;
			case 2: memory_scan(mem, pid); break;
			case 3: scan_diff(mem, pid); break;
			case 4: reset_diff(); break;
			case 5: memory_write(pid); break;
			case 6: refresh(argv[1], &list); break;
			case 7: change_type(); break;
			case 0: running = 0; break;
			default: printf("Invalid option\n"); break;
		}
	}

	if (mem)
		free_mem(mem);
	return 0;
}