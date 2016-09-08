#include "liblash/lash_tree3_dump.h"
#include "liblash/lash_tree3.h"
#include <stdio.h>
#include <stdlib.h>

unsigned int lash_tree_dump_monitor_capacity;
unsigned int lash_tree_dump_monitor_count;

void lash_treeDump(lash_tree_t *tree, char *comment) {
	int i;
	int j;

	lash_tree_t *currenttree;
	
	for (i = 0; i < lash_tree_dump_monitor_count; i++) {
		currenttree = *(lash_tree_dump_monitor.tree + i);
		if (tree != NULL)
			if (tree != currenttree)
				continue;
			
		printf("'%s' size %d", *(lash_tree_dump_monitor.name + i), currenttree->count);
		if (comment != NULL)
			printf(" (%s)", comment);
		printf(":\n");
		
		for (j = 0; j < currenttree->count; j++) {
			printf("i:%2d v:%3li\n", j, (long int)**(currenttree->key + j));
		}
	}
}

int lash_treeDumpInit(unsigned int count) {
	
	lash_tree_dump_monitor.tree = (lash_tree_t**)malloc(sizeof(lash_tree_t*) * count);
	if (lash_tree_dump_monitor.tree == NULL)
		return 1;
		
	lash_tree_dump_monitor.name = (char**)malloc(sizeof(char) * 128 * count);
	if (lash_tree_dump_monitor.name == NULL)
		return 1;
	
	lash_tree_dump_monitor_capacity = count;
	lash_tree_dump_monitor_count = 0;
	
	return 0;	
}

int lash_treeDumpAdd(lash_tree_t *tree, char *name) {
	if (lash_tree_dump_monitor_count == lash_tree_dump_monitor_capacity)
		return 1;
		
	*(lash_tree_dump_monitor.tree + lash_tree_dump_monitor_count) = tree;
	*(lash_tree_dump_monitor.name + lash_tree_dump_monitor_count) = name;	
	
	lash_tree_dump_monitor_count++;
	
	return 0;
}

void lash_treeDumpFree() {
	if (lash_tree_dump_monitor.tree != NULL)
		free(lash_tree_dump_monitor.tree);
		
	if (lash_tree_dump_monitor.name != NULL)
		free(lash_tree_dump_monitor.name);
}
