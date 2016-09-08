#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "liblash/lash_tree3.h"
#include "liblashgame/lash_game_path_simple.h"
#include "liblashgame/lash_game_standard.h"

/**
 * \todo unitsize should be defined somewhere centrally. Map can be candidate
 * \warning the process step only checks for existence of existing spaces in the openlist, while theoretically a space can be shifted from openlist with nextstep without being added to the closedlist
 * 
 */
 
lash_path_simple_t * lash_pathSimpleInit(lash_path_simple_t *path, lash_map_simple_t *map, const lash_game_map_index_t offsetindex, const lash_game_map_index_t targetindex) {
	unsigned int unitsize = 1;
	unsigned int treecapacity = (unsigned int)*(map->w) * *(map->h) * 2;
	
	lash_game_coords_t offsetcoords;
	lash_game_coords_t targetcoords;
	
	lash_indexToCartesian(&offsetcoords, map->w, map->h, &unitsize, &offsetindex);
	lash_indexToCartesian(&targetcoords, map->w, map->h, &unitsize, &targetindex);
	
	path->w = map->w;
	path->h = map->h;
	path->target = targetindex;
	path->source = offsetindex;

	// enough for the whole world if necessary, consider making a realloc routine for these
	path->opentree = lash_treeInit(path->opentree, treecapacity); 
	if (path->opentree == NULL)
		return NULL;
		
	path->closedtree = lash_treeInit(path->closedtree, treecapacity);
	if (path->closedtree == NULL)
		return NULL;
		
	path->spaces = (lash_path_simple_space_t*)malloc(sizeof(lash_path_simple_space_t) * treecapacity);
	if (path->spaces == NULL)
		return NULL;
	
	path->space_capacity = treecapacity;
	path->space_count = 0;
	
	if (lash_pathSimpleUpdateSpace(path, path->spaces, NULL, offsetindex, 0) == NULL)
		return NULL;
		
	path->space_count = 1;
	
	lash_treePush(path->opentree, path->spaces, NULL);
	
	return path;
}

lash_path_simple_space_t * lash_pathSimpleAddSpace(lash_path_simple_t *path) {
	if (path->space_count == path->space_capacity)
		return NULL;
	
	path->space_count++;
	
	return (path->spaces + path->space_count - 1);
}

lash_path_simple_t * lash_pathSimpleUpdateSpace(lash_path_simple_t *path, lash_path_simple_space_t *currentspace, lash_path_simple_space_t *parent, const lash_game_map_index_t currentindex, const unsigned int g) {
	/// \todo check memory cap, increase if necessary
	unsigned int unitsize = 1;
	lash_game_coords_t targetcoords, currentcoords;

	currentspace->g = g;
	currentspace->parent = parent;
	lash_indexToCartesian(&targetcoords, path->w, path->h, &unitsize, &path->target);
	lash_indexToCartesian(&currentcoords, path->w, path->h, &unitsize, &currentindex);
	currentspace->h = lash_getManhattanMagnitudeFromCartesian(targetcoords, currentcoords);
	currentspace->f = (currentspace->h * 10) + currentspace->g; // f score
	currentspace->index = currentindex;
	
	return path;
}

int lash_pathSimpleClose(lash_path_simple_t *path, lash_path_simple_space_t *space) {
	return lash_treePush(path->closedtree, space, NULL);
}

/**
 * \todo weighted movement calculations for semi-passable spaces
 */
unsigned int lash_pathSimpleStepProcess(lash_path_simple_t *path, lash_map_simple_t *map, lash_path_simple_space_t *centerspace, const lash_path_simple_obstacle_t *obstacleslist, const unsigned char obstaclescount) {
	int newx;
	int newy;
	unsigned int unitsize = 1;
	unsigned int freespaces = 0;
	lash_game_coords_t centercoords, newcoords;
	
	lash_indexToCartesian(&centercoords, map->w, map->h, &unitsize, (lash_game_map_index_t*)&centerspace->index);
	
	// close the current index
	lash_pathSimpleClose(path, centerspace);
	
	//printf("Processing index %u\n", centerspace->index);
	
	// loop through adjacent vertical spaces
	for (newy = centercoords.y - 1; newy <= (int)(centercoords.y + 1); newy++) {
	
		// loop through adjacent horizontal spaces
		for (newx = centercoords.x - 1; newx <= (int)(centercoords.x + 1); newx++) {
			
			//printf("x %u y %u ", newx, newy);
			
			// store the movement modifier
			// this is currently only binary - passable or non passable
			float modifier;
			
			// store the new space for editing
			lash_path_simple_space_t *newspace;
			
			// newindex -1 means invalid index
			lash_game_map_index_t newindex = -1;
			
			// initialize the score with diagonal movement
			// change it further down if movement is straight
			// the score is the weight of the movement from the path's startindex to current centerspace, plus this new movement
			unsigned char newscore = centerspace->g + LASH_GAME_PATH_SIMPLE_SCORE_DIAGONAL;
			
			// if out of bounds then do nothing
			if (newx < 0 || newx > *(map->w) - 1 || newy < 0 || newy > *(map->h) -1) {
				//printf("(out of bounds)\n");
				continue;
			}
				
			// lash_game_coords_t handle only unsigned values, so the negatives must be handled with local ints
			newcoords.x = newx;
			newcoords.y = newy;
			
			// find the index of the current adjacent space
			lash_cartesianToIndex(&newindex, map->w, map->h, &unitsize, &newcoords);
			//printf("i %u: ", (unsigned int)newindex);
			
			// modifier related
			modifier = lash_pathSimpleCheckModifier(&(map->layer_path + newindex)->val, obstacleslist, obstaclescount);
			// if not passable, do nothing with this space
			if (modifier == 0.f) {
				//printf("impassable\n");
				continue;
			}
			
			// if the space is already in the closedlist, do nothing with this space
			if (lash_pathSimpleFindIndexInTree(path->closedtree, newindex)) {
				//printf("closed\n");
				continue;
			}
			
			// update the newscore if movement is straight	
			if (newcoords.y == centercoords.y) {
				// if same square as center
				// this should never be evaluated, as the current square should be on the closed list
				if (newcoords.x == centercoords.x) {
					continue;
				}
				// right of left
				newscore = centerspace->g + LASH_GAME_PATH_SIMPLE_SCORE_STRAIGHT;
			} else if (newcoords.x == centercoords.x) {
				// up or down
				newscore = centerspace->g + LASH_GAME_PATH_SIMPLE_SCORE_STRAIGHT;
			}
			 
			// if we made it this far, it means that the space should be evaluated
			// that means process it for the openlist
			newspace = lash_pathSimpleFindIndexInTree(path->opentree, newindex);
			
			// if the space is not already in the openlist, add it and update it
			if (newspace == NULL) {
				newspace = lash_pathSimpleAddSpace(path);
				lash_pathSimpleUpdateSpace(path, newspace, centerspace, newindex, newscore);
				lash_treePush(path->opentree, newspace, NULL);
			// if not update the existing space
			// compare to the old score
			// if old score is lower change back
			} else {
				lash_path_simple_space_t oldspace;
				memcpy(&oldspace, newspace, sizeof(lash_path_simple_space_t));
				lash_pathSimpleUpdateSpace(path, newspace, centerspace, newindex, newscore);
				if (newspace->f > oldspace.f)
					memcpy(newspace, &oldspace, sizeof(lash_path_simple_space_t));
			}
			
			//printf("score %u\n", newspace->f);
			
			freespaces++;
		}
	}
	
	return freespaces;
}


lash_path_simple_space_t * lash_pathSimpleNext(lash_path_simple_t *path, lash_path_simple_space_t **space) {
	
	if (path->opentree->count == 0) {
		*space = NULL;
	}

	lash_treeShift(path->opentree, (void*)space);
		
	return (lash_path_simple_space_t*)space;
}

// 1.0 = full movement, 0.0 = impassable
/**
 * \todo check if bitwise check against modifier is faster than division
 */
float lash_pathSimpleCheckModifier(const lash_map_simple_layer_item_t *layeritem, const lash_path_simple_obstacle_t *obstacleslist, const unsigned char obstaclescount) {
	if (layeritem == NULL || obstacleslist == NULL || obstaclescount == 0)
		return 1;
		
	int i;
	for (i = 0; i < obstaclescount; i++) {
		if (*layeritem == (obstacleslist+i)->val)
			return (obstacleslist+i)->modifier / (float)LASH_GAME_PATH_SIMPLE_OBSTACLE_MODIFIER_NONE;
	}
	return 1;
}

void lash_pathSimpleFree(lash_path_simple_t *path) {
	free(path->opentree);
	free(path->closedtree);
	free(path->spaces);
}


lash_path_simple_space_t * lash_pathSimpleFindIndexInTree(lash_tree_t *tree, lash_game_map_index_t index) {
	int i;
	lash_path_simple_space_t *checkspace = NULL;
	
	if (tree == NULL)
		return NULL;
		
	for (i = 0; i < tree->count; i++) {
		checkspace = (lash_path_simple_space_t*)*(tree->item + i);
		if (checkspace->index == index)
			return checkspace;
	}
	
	return NULL;
}

