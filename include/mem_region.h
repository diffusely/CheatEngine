#ifndef MEM_REGION
#define MEM_REGION

typedef struct MemRegion {
    unsigned long start;
    unsigned long end;
    char perms[5];
    struct MemRegion *next;
} MemRegion;

void push_back(MemRegion **head, MemRegion *node);
void push_front(MemRegion **head, MemRegion *node);
MemRegion *pop_back(MemRegion **head);
MemRegion *pop_front(MemRegion **head);
void free_list(MemRegion *head);
void print_mem_region(MemRegion *head);
MemRegion *create_node(unsigned long start, unsigned long end, char *perms);

#endif