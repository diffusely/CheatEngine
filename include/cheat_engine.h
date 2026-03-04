#ifndef CHEAT_ENGINE
#define CHEAT_ENGINE

#include "mem_region.h"
#include "lib.h"

#define MAX_PIDS 100

typedef struct {
	int pids[MAX_PIDS];
	char names[MAX_PIDS][256];
	int count;
} PidList;


int get_pid_list(char *name, PidList *list);
int select_pid(PidList *list);
int run(char **argv);

#endif