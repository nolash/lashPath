// test the pathfinder
// this test should be split up for the add space and other individual items


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "liblashgame/lash_game_path_simple.h"
#include "liblashgame/lash_game_map.h"
#include "liblashgame/lash_game_standard.h"
#include "liblash/lash_tree3_dump.h"

// segfault if input formats with commas aren't right
// the y coords should be checked for valid return from strtok before atoi
// does not check bounds of coords

unsigned int _unitsize = 1;


void dumpmappathlayer(lash_map_simple_t *map) {
	lash_game_coords_t tmpcoords;
	lash_game_map_index_t tmpidx;
	for (tmpcoords.y = 0; tmpcoords.y < *map->h; tmpcoords.y++) {
		for (tmpcoords.x = 0; tmpcoords.x < *map->w; tmpcoords.x++) {
			lash_cartesianToIndex(&tmpidx, map->w, map->h, &_unitsize, &tmpcoords);
			printf("%02i ", (int)lash_mapSimpleLayerPeek(map->layer_path, tmpidx));
		}
		printf("\n");
	}
	printf("\n");	
}

int main(int argc, char *argv[]) {
	if (argc < 5) {
		printf("Usage: %s world_width world_height startx,starty targetx,targety obstacleindexval[,obstacleoindexval[...]] [obstaclex,obstacley,obstacleitemval [obstaclex,obstacley,obstacleitemval ...]]\n", argv[0]);
		return 1;
	}
	
	int i;
	
	lash_game_coords_t offset_coords;
	lash_game_coords_t target_coords;
	lash_game_map_index_t offset_idx;
	lash_game_map_index_t target_idx;
	lash_game_map_index_t *obstacles;
	
	unsigned int w = atoi(argv[1]);
	unsigned int h = atoi(argv[2]);
	
	offset_coords.x = atoi(strtok(argv[3], ","));
	offset_coords.y = atoi(strtok(NULL, ","));
	lash_cartesianToIndex(&offset_idx, &w, &h, &_unitsize, &offset_coords);
	
	target_coords.x = atoi(strtok(argv[4], ","));
	target_coords.y = atoi(strtok(NULL, ","));
	lash_cartesianToIndex(&target_idx, &w, &h, &_unitsize, &target_coords);

	printf("World: w%d x h%d\nOffset: x%d,y%d (idx %d)\nTarget: x%d,y%d (idx %d)\n", w, h, offset_coords.x, offset_coords.y, (int)offset_idx, target_coords.x, target_coords.y, (int)target_idx);
	printf("Obstacle definitions: ");

	int obstaclemastercount = 0;
	char *tmpobstaclemastersource = (char*)malloc(strlen(argv[5]));
	strcpy(tmpobstaclemastersource, argv[5]);
	char *tmpobstaclemaster = strtok(tmpobstaclemastersource, ",");
	while (tmpobstaclemaster != NULL) {
		obstaclemastercount++;
		tmpobstaclemaster = strtok(NULL, ",");
	}
	
	lash_path_simple_obstacle_t *obstaclemasterlist = (lash_path_simple_obstacle_t*)malloc(sizeof(lash_path_simple_obstacle_t) * obstaclemastercount);
	i=0;
	strcpy(tmpobstaclemastersource, argv[5]);
	char *tmpobstaclemaster2 = strtok(tmpobstaclemastersource, ",");
	while (tmpobstaclemaster2 != NULL) {
		(obstaclemasterlist + i)->val = atoi(tmpobstaclemaster2);
		(obstaclemasterlist + i)->modifier = LASH_GAME_PATH_SIMPLE_OBSTACLE_MODIFIER_FULL;
		printf("%d ", (obstaclemasterlist + i)->val);
		tmpobstaclemaster2 = strtok(NULL, ",");
		i++;
	}
	free(tmpobstaclemastersource);
	printf("\n");
	

	lash_map_simple_t map;
	lash_mapSimpleInit(&map, &w, &h);

	if (argc > 6) {
		obstacles = (lash_game_map_index_t*)malloc(sizeof(lash_game_map_index_t) * (argc - 6));
		
		for (i = 0; i < argc - 6; i++) {
			lash_game_coords_t obstacle_coords;	
			lash_map_simple_layer_item_t obstacleval;
			obstacle_coords.x = atoi(strtok(argv[i + 6], ","));
			obstacle_coords.y = atoi(strtok(NULL, ","));
			obstacleval = atoi(strtok(NULL, ","));
			lash_cartesianToIndex((obstacles + i), &w, &h, &_unitsize, &obstacle_coords);
			lash_mapSimpleLayerPoke(map.layer_path, *(obstacles +i), obstacleval);
			printf("Obstacle #%d: x%d,y%d (idx %d) of type %d\n", i + 1, obstacle_coords.x, obstacle_coords.y, (int)*(obstacles + i), obstacleval);
		}
	}
	
	lash_path_simple_t path;
	lash_path_simple_space_t *space;
	
	lash_pathSimpleInit(&path, &map, offset_idx, target_idx);
	
	lash_treeDumpInit(1);
	lash_treeDumpAdd(path.closedtree, "closedtree");
	
	lash_pathSimpleNext(&path, &space);

	printf("Map path layer dump:\n");
	dumpmappathlayer(&map);
	printf("---\nTrying step...\n");
	i = 0;
	
	while (space->index != path.target) {
		
		printf("Step %d with opentree index %p spaceindex %d target %d\n", ++i, *path.opentree->key, (int)space->index, (int)path.target);
		lash_pathSimpleStepProcess(&path, &map, space, obstaclemasterlist, obstaclemastercount);
		lash_pathSimpleNext(&path, &space);
		if (space == NULL)
			break;
	}
	
	return 0;
}
