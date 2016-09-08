#include "../lash_game_path.h"
#include "../lash_game_map.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void dumpspaces (lash_path_simple_t *path) {
	int i;
	printf("Spaces dump:\n");
	for (i = 0; i < path->spaces_position; i++) {
		printf("%i=i%i,f%i ", i, (path->spaces+i)->index, (path->spaces+i)->f);
	}
	printf("\n");
}


int dumptree(lash_tree_t *tree, char *treename) {
		printf("Tree %s is now size %d: ", treename, tree->position);
		int j;
		//long int lastn = 0;
		int falsesort = 0;
		long int nn, nc;
		unsigned int nl;
		for (j = 1; j <= tree->position; j++) {
			int ti = *(tree->idx + j - 1);
			nn = *(tree->heap + ti);
			nl = *(tree->local + ti);
			nc = *(tree->heap + *(tree->idx + (int)(j / 2) - 1));
			printf("#%u:h%li,l%u ", j, nn, nl);
			if (nn < nc && j > 1)
				falsesort = 1;
		}
		if (falsesort == 1) {
			printf("SORTERROR!\n");
			return 1;
		}
		printf("\n");	
		return 0;
}

void dumpmappathlayer(lash_map_simple_t *map, unsigned int unitsize, int position, int target, lash_path_simple_space_t *finalpath, unsigned int finalpathcount) {
	lash_game_coords_t tmpcoords;
	lash_game_map_index_t tmpidx;
	for (tmpcoords.y = 0; tmpcoords.y < *map->h; tmpcoords.y++) {
		for (tmpcoords.x = 0; tmpcoords.x < *map->w; tmpcoords.x++) {
			lash_cartesianToIndex(&tmpidx, map->w, map->h, unitsize, &tmpcoords);
			if (tmpidx == position) {
				printf("XX ");
			} else if (tmpidx == target) {
				printf("-- ");
			} else if (finalpath != NULL) {
				int i = 0;
				int onpath = 0;
				for (i = 0; i <= finalpathcount; i++) {
					if (tmpidx == (finalpath+i)->index && onpath == 0) {
						printf(".. ");
						onpath = 1;
					}
				}
				if (onpath == 0) {
					printf("%02i ", (int)lash_mapSimpleLayerPeek(map->layer_path, tmpidx));	
				}
			} else {
				printf("%02i ", (int)lash_mapSimpleLayerPeek(map->layer_path, tmpidx));
			}
		}
		printf("\n");
	}
	printf("\n");	
}

int main(int argc, char **argv) {
	
	lash_map_simple_t map;
	lash_path_simple_t path;
	lash_path_simple_space_t *space;
	
	int w = -1;
	int h = -1;
	int idx = -1;
	int r = -1;
	char arg = -1;
	
	int i = 0;
	int xclip = 0;
	int yclip = 0;
	
	unsigned int indexunit;
	unsigned int dleft;
	unsigned int dright;
	unsigned int dtop;
	unsigned int dbottom;
	
	unsigned int ctop;
	unsigned int cbottom;
	unsigned int cleft;
	unsigned int cright;
	
	unsigned int spacestotal;
	lash_game_map_index_t *spaces;
	unsigned int spacescount;
	
	lash_game_map_index_t target;
	
	unsigned char obstaclestotal = 2;
	
	lash_path_simple_obstacle_t *obstacles = (lash_path_simple_obstacle_t*)malloc(sizeof(lash_path_simple_obstacle_t) * 3);
	obstacles->val = 0;
	obstacles->modifier = LASH_GAME_PATH_SIMPLE_OBSTACLE_MODIFIER_NONE;
	(obstacles+1)->val = 1;
	(obstacles+1)->modifier = LASH_GAME_PATH_SIMPLE_OBSTACLE_MODIFIER_FULL;
	(obstacles+2)->val = 2;
	(obstacles+2)->modifier = LASH_GAME_PATH_SIMPLE_OBSTACLE_MODIFIER_HALF;
	
	lash_path_simple_space_t *finalpath;
	
	time_t t;
	
	srand((unsigned) time(&t));
		
	while ((arg = getopt(argc, argv, "w:h:i:r:")) != -1) {
		switch(arg) {
			case 'w':
				w = atoi(optarg);
				break;
			case 'h':
				h = atoi(optarg);
				break;
			case 'i':
				idx = atoi(optarg);
				break;
			case 'r':
				r = atoi(optarg);
				break;
		}
	}
	
	if (w < 0 || h < 0 || idx < 0 || r < 0) {
		fprintf(stderr, "All options not set, exiting ...\n");
		return 1;
	}
	
	printf("Width %d Height %d index %d radius %d\n", w, h, idx, r);
		
	lash_mapSimpleInit(&map, &w, &h);
	
	for (i = 0; i < w * h; i++) {
		if (i != idx)
			lash_mapSimpleLayerPoke(map.layer_path, i, rand() % 2);
	}
	
	dumpmappathlayer(&map, 1, idx, -1, NULL, 0);
	
	dtop = (int)floor(idx / w);
	dbottom = h - dtop - 1;
	dleft = idx - dtop * w;
	dright = ((dtop + 1) * w) - idx - 1;
	
	ctop = r > dtop ? dtop : r;
	cbottom = r > dbottom ? dbottom : r;
	cleft = r > dleft ? dleft : r;
	cright = r > dright ? dright : r;
	
	spacestotal = (cright + cleft + 1) * (cbottom + ctop + 1);
	
	printf("Spaces to left edge %d clipping %d\n", dleft, cleft);
	printf("Spaces to right edge %d clipping %d\n", dright, cright);
	printf("Spaces to upper edge %d clipping %d\n", dtop, ctop);
	printf("Spaces to lower edge %d clipping %d\n", dbottom, cbottom);
	printf("Total spaces in clip %d\n", spacestotal);
		
	spaces = (lash_game_map_index_t*)malloc(sizeof(lash_game_map_index_t) * spacestotal);
	spacescount = 0;
	
	for (yclip = idx - (ctop * w); yclip <= idx + (cbottom * w); yclip += w) {
		for (xclip = yclip - cleft; xclip <= yclip + cright; xclip++) {
			lash_map_simple_layer_item_t item = lash_mapSimpleLayerPeek(map.layer_path, xclip);
			if (xclip == idx) {
				printf("XX ");
			} else {
				printf("%.2d ", item);
				
				// here insert obstacleslist check
				//if (item == 0) {
				if (lash_pathSimpleCheckModifier(&item, obstacles, obstaclestotal) > 0.0) {
					*(spaces + spacescount) = xclip;
					spacescount++;
				} else {
					//fprintf(stderr, "item %d idx %d is impassable\n", item, xclip);
				}
			}
		}
		printf("\n");
	}
	
	// spacescount + 1 but original position is also reckoned not open
	printf("Open spaces %d\n", spacescount);
	
	if (spacescount == 0) {
		return 0;
	}
	
	target = rand() % spacescount;
	printf("Target pointer pos is %d\n", target);
	printf("Target is %d\n", *(spaces + target));
	dumpmappathlayer(&map, 1, idx, *(spaces + target), NULL, 0);

	lash_pathSimpleInit(&path, &map, idx, *(spaces + target));
	space = lash_pathSimpleNext(&path);
	space = lash_pathSimpleStepProcess(&path, &map, space, obstacles, obstaclestotal);
	while (space->index != path.target) {
			space = lash_pathSimpleStepProcess(&path, &map, space, obstacles, obstaclestotal);
			if (space == NULL) {
				printf("No path possible!!\n");
				break;
			}
	} 
		
	i = 0;
	
	finalpath = (lash_path_simple_space_t*)malloc(sizeof(lash_path_simple_space_t) * (path.closedtree->position + 1));
	*(finalpath + 0) = *space;
	if (finalpath->index != path.target) {
		printf("No path possible!!\n");
	} else {
		do {
			space = (finalpath + i)->parent;
			if (space != NULL) {
				i++;
				*(finalpath + i) = *space;
			}
		} while (space != NULL);
	}
	
	dumpmappathlayer(&map, 1, idx, *(spaces + target), finalpath, i);
	
	
	lash_pathSimpleFree(&path);
	lash_mapSimpleFree(&map);
	free(spaces);
	free(finalpath);
	free(obstacles);
	
	return 0;
	
}

