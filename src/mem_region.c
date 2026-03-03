#include "mem_region.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

MemRegion *create_node(unsigned long start, unsigned long end, char *perms)
{
    MemRegion *node = malloc(sizeof(MemRegion));
    node->start = start;
    node->end = end;
    strcpy(node->perms, perms);
    node->next = NULL;
    return node;
}

void push_back(MemRegion **head, MemRegion *node)
{
    node->next = NULL;

    if (!*head) {
        *head = node;
        return;
    }

    MemRegion *curr = *head;
    while (curr->next)
        curr = curr->next;
    curr->next = node;
}

void push_front(MemRegion **head, MemRegion *node)
{
    node->next = *head;
    *head = node;
}

MemRegion *pop_back(MemRegion **head)
{
    if (!*head) return NULL;

    if (!(*head)->next) {
        MemRegion *temp = *head;
        *head = NULL;
        return temp;
    }

    MemRegion *curr = *head;
    while (curr->next->next)
        curr = curr->next;

    MemRegion *temp = curr->next;
    curr->next = NULL;
    return temp;
}

MemRegion *pop_front(MemRegion **head)
{
    if (!*head) return NULL;

    MemRegion *temp = *head;
    *head = (*head)->next;
    return temp;
}

void free_list(MemRegion *head)
{
    while (head) {
        MemRegion *temp = head;
        head = head->next;
        free(temp);
		temp = NULL;
    }
}

void print_mem_region(MemRegion *head)
{
    int i = 0;
    while (head) {
        printf("[%d] 0x%lx - 0x%lx (%s)\n", i++, 
			head->start, head->end, head->perms);
        head = head->next;
    }
}
