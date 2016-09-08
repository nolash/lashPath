#include <stdlib.h>
#include <stdio.h>

#include "liblashgame/lash_game_path_simple.h"
#include "liblashgame/lash_game_map.h"

#include "liblash/lash_tree3_dump.h"
#include "liblash/lash_tree3.h"

int main() {
	
	lash_path_simple_t path;
	lash_map_simple_t map;
	lash_path_simple_space_t *space;
	unsigned int w = 20;
	unsigned int h = 10;
	lash_game_map_index_t startindex = 50;
	lash_game_map_index_t nextindex = 60;
	lash_game_map_index_t endindex = 150;
	lash_path_simple_space_t *resultspace;
	
	if (lash_mapSimpleInit(&map, &w, &h) == NULL)
		return 1;
		
	if (lash_pathSimpleInit(&path, &map, startindex, endindex) == NULL)
		return 1;
	
	lash_pathSimpleNext(&path, &space);
	lash_pathSimpleClose(&path, *(path.opentree)->item);
	resultspace = lash_pathSimpleFindIndexInTree(path.closedtree, startindex);
	
	space = lash_pathSimpleAddSpace(&path);
	lash_pathSimpleUpdateSpace(&path, space, resultspace, nextindex, resultspace->g + 14);
	lash_treePush(path.opentree, space, NULL);
	
	resultspace = lash_pathSimpleFindIndexInTree(path.opentree, nextindex);
	
	return 0;
}
