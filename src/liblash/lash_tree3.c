#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "liblash/lash_tree3.h"


lash_tree_t *lash_treeInit(lash_tree_t *tree, const unsigned int size) {
	tree = (lash_tree_t*)malloc(sizeof(lash_tree_t));
	tree->key = (lash_tree_key_t**)malloc(sizeof(lash_tree_key_t*) * size + 1);
	tree->item = (void*)malloc(sizeof(void*) * size + 1);
	
	if (tree->key == NULL || tree->item == NULL)
		return NULL;
		
	tree->count = 0;
	//tree->shiftpos = 0;
	tree->capacity = size;
	//tree->foldbackthreshold = tree->capacity / 2;
	
	return tree;
}

void lash_treeFree(lash_tree_t *tree) {
	if (tree == NULL)
		return;
		
	if (tree->key != NULL)
		free(tree->key);
	
	if (tree->item != NULL)
		free(tree->item);
	
	free(tree);
}

// key is optional, if NULL is passed, then tree assumes a lash_tree_key_t at first address of item pointer
int lash_treePush(lash_tree_t *tree, void *item, lash_tree_key_t *key) {
	//printf("%p", item);
	
	if (tree->count == tree->capacity)
		return -1;
	
	tree->count++;
	
	if (key != NULL)
		*(tree->key + tree->count - 1) = key;
	else
		*(tree->key + tree->count - 1) = (lash_tree_key_t*)item;
		
	*(tree->item + tree->count - 1) = item;
	
	_lash_treeProcess(tree, tree->count, -1);
	
	return tree->count;
}

int lash_treeShift(lash_tree_t *tree, void **item) {
	return lash_treeRemove(tree, 0, item);
}	

int lash_treeRemove(lash_tree_t *tree, unsigned int position, void **item) {

	if (tree->count == 0)
		return -1;
	
	// retrieve the values from the position from the argument
	*item = *(tree->item + position);
	
	// move the values from the end of the tree to the position from the argument
	*(tree->key + position) = *(tree->key + tree->count - 1);
	*(tree->item + position) = *(tree->item + tree->count - 1);
	
	// reduce the count of the tree
	tree->count--;
	
	// iterate the new value down the tree to its proper place
	_lash_treeProcess(tree, position + 1, 1);
	
	return tree->count;
}

int _lash_treeProcess(lash_tree_t *tree, unsigned int currentposition, int direction) {

	// towards end of pointer is downwards in the tree
	// the direction to sort, -1 is from end towards start, 1 is from startposition towards end
	
	while (direction != 0) {
		
		lash_tree_key_t *currentkey = *(tree->key + currentposition - 1);
		lash_tree_key_t *nextkey = NULL;
		lash_tree_key_t *swapkey;
		//lash_tree_key_t swapkey;
		void *swapitem;;
		void *currentitem;
		void *nextitem;
		
		unsigned int nextposition;
	
		switch(direction) {
			
			// if there is still room to ascend in the tree, that is if the current position isn't 0 already.
			case -1:
				if (currentposition == 1) {
					direction = 0;
				} else {
					nextposition = currentposition >> 1;	
					nextkey = *(tree->key + nextposition - 1);
					if (*currentkey < *nextkey) {
						/*swapkey = *currentkey;
						*currentkey = *nextkey;
						*nextkey = swapkey;*/
						
						currentitem = *(tree->item + currentposition - 1);
						nextitem = *(tree->item + nextposition - 1);
						
						swapkey = currentkey;
						*(tree->key + currentposition - 1) = nextkey;
						*(tree->key + nextposition - 1) = swapkey;
						
						// ORIGINALLY IT SEEMED LIKE WHEN THE ADDRESS WAS THE SAME THAT THIS WOULD SHIFT THINGS AROUND TWICE
						// NOW IT WORKS IF THE ADDRESS OF THE KEY AND THE FIRST ADDRESS OF THE ITEM ARE THE SAME
						// if the position of the key is not in the first address of the item we need to move it separately
						// note that swapkey now holds the original currentkey
						//if ((lash_tree_key_t *)currentitem != swapkey) {
							swapitem = currentitem;
							*(tree->item + currentposition - 1) = nextitem;
							*(tree->item + nextposition - 1) = swapitem;
						//}
						
						currentposition = nextposition;
					} else {
						direction = 0;
					}
				}
				break;
			
			// if there is still room from the current position to descend one level in the tree):
			// level 0 has 2^0 = entry 1
			// level 1 has 2^1 = entries 2-3
			// level 2 has 2^2 = entries 4-7
			// if we're at position 3, the check will be 3*2 = 6, which is within bounds of level 2, and points to elements 6 and 7 (the children of 3)
			// If we're at position 4, the check will be 4*2 = 8, which is out of bounds of level 2.
			
			case 1:
				nextposition = currentposition << 1;	
				
				if (nextposition <= tree->count) {
					
					// compare both the child values, to pick the higher one
					nextkey = *(tree->key + nextposition - 1);
					if (*nextkey > **(tree->key + nextposition)) {
						nextkey = *(tree->key + nextposition); 
						nextposition++;
					}
					
					// compare the values, set nextkey back to null to stop the swapping if the value is not higher
					if (*currentkey <= *nextkey) {
						nextkey = NULL;
					}
				
					if (nextkey != NULL) {
						/*swapkey = *currentkey;
						*currentkey = *nextkey;
						*nextkey = swapkey;*/
						
						currentitem = *(tree->item + currentposition - 1);
						nextitem = *(tree->item + nextposition - 1);
						
						swapkey = currentkey;
						*(tree->key + currentposition - 1) = nextkey;
						*(tree->key + nextposition - 1) = swapkey;
						
						// ORIGINALLY IT SEEMED LIKE WHEN THE ADDRESS WAS THE SAME THAT THIS WOULD SHIFT THINGS AROUND TWICE
						// NOW IT WORKS IF THE ADDRESS OF THE KEY AND THE FIRST ADDRESS OF THE ITEM ARE THE SAME
						// if the position of the key is not in the first address of the item we need to move it separately
						// note that swapkey holds the original currentkey value
						//if ((lash_tree_key_t *)currentitem != swapkey) {
							swapitem = currentitem;
							*(tree->item + currentposition - 1) = nextitem;
							*(tree->item + nextposition - 1) = swapitem;
						//}
						
						currentposition = nextposition;
					} else {
						direction = 0;
					}
				} else {
					direction = 0;
				}
				break;
			default:
				direction = 0;
		}
	}	
	
	return currentposition;
}
