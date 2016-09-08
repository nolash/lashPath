#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "liblash/lash_tree3_dump.h"
#include "liblash/lash_debug_timer.h"

#include "liblashgame/lash_game_path_simple.h"
#include "liblashgame/lash_game_map.h"
#include "liblashgame/lash_game_standard.h"

#define LASH_X_TEST_COUNT 1000

int loadMap(char *filename, int *w, int *h, int *s, int *t, char **savemap) {
	FILE *f;
	size_t fpos;
	
	unsigned char buffer[1024];
	char *tmpmap;
	
	f = fopen(filename, "rb");
	
	if (f == NULL)
		return 1;
		
	fpos = fread(buffer, 1, 8, f);
	
	*w = (int)(buffer[0] | buffer[1] << 8);
	*h = (int)(buffer[2] | buffer[3] << 8);
	*s = (int)(buffer[4] | buffer[5] << 8);
	*t = (int)(buffer[6] | buffer[7] << 8);
	
	tmpmap = (char*)calloc(*w * *h, sizeof(char));
	if (tmpmap == NULL)
		return 1;
	
	fseek(f, fpos, SEEK_SET);
	
	fpos += fread(buffer, 1, (size_t)*w, f);
	
	// if data in file is not aligned to w this will cause a segfault
	while (!feof(f)) {
		memcpy((tmpmap + fpos - *w - 8), buffer, (size_t)*w);
		fseek(f, fpos, SEEK_SET);
		fpos += fread(buffer, 1, (size_t)*w, f);
		
	}
	
	fclose(f);
	
	*savemap = tmpmap;
	
	return 0;
}

void freeMap(char *savemap) {
	if (savemap != NULL)
		free(savemap);
}

void dumpmappathlayer(lash_map_simple_t *map, unsigned int unitsize, int start, int target, int position, lash_path_simple_space_t *finalpath, unsigned int finalpathcount) {
	lash_game_coords_t tmpcoords;
	lash_game_map_index_t tmpidx;
	for (tmpcoords.y = 0; tmpcoords.y < *map->h; tmpcoords.y++) {
		for (tmpcoords.x = 0; tmpcoords.x < *map->w; tmpcoords.x++) {
			lash_cartesianToIndex(&tmpidx, map->w, map->h, &unitsize, &tmpcoords);
			if (tmpidx == start) {
				printf("XX ");
			} else if (tmpidx == target) {
				printf("-- ");
			} else if (tmpidx == position) {
				printf("?? ");
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
	fflush(stdout);
}

int main(int argc, char **argv) {
	
	lash_map_simple_t map;
	lash_path_simple_t path;
	lash_path_simple_space_t *space;
	lash_debug_timer_t timer;
	
	int w = -1;
	int h = -1;
	int idx = -1;
	int savedtarget = -1;
	int r = -1;
	char arg = -1;
	char *filename = NULL;
	
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
	
	unsigned int pathopenstepsresult;
	int impossible = 0;
	unsigned int attemptedsteps;
	unsigned int testcount = LASH_X_TEST_COUNT;
	
	lash_game_map_index_t target;
	lash_game_map_index_t targetglobalindex;
	
	char *savedmap = NULL;
	
	unsigned char obstaclestotal = 2;
	
	int dumpeverystep = 0;
	
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
		
	while ((arg = getopt(argc, argv, "w:h:i:r:f:c:d")) != -1) {
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
			case 'c':
				testcount = atoi(optarg);
				break;
			case 'd':
				dumpeverystep = 1;
				break;
			case 'f':
				filename = optarg;
				break;
		}
	}
	
	if (filename != NULL) {
		if (strcmp(filename, "")) {
			if (loadMap(filename, &w, &h, &idx, &savedtarget, &savedmap))
				return 1;
			lash_mapSimpleInit(&map, &w, &h);
			for (i = 0; i < w * h; i++)
				lash_mapSimpleLayerPoke(map.layer_path, i, *(savedmap + i));
		}
	}
	
	if (savedmap != NULL) {
		freeMap(savedmap);
	} else {
		if (w < 0 || h < 0 || r < 0) {
			fprintf(stderr, "All options not set, exiting ...\n");
			return 1;
		} else {
			lash_mapSimpleInit(&map, &w, &h);
	
			for (i = 0; i < w * h; i++) {
				if (i != idx)
					lash_mapSimpleLayerPoke(map.layer_path, i, rand() % 2);
			}
		}
	}
	
	printf("Width %d Height %d index %d radius %d target %d\n", w, h, idx, r, savedtarget);
	
	while (idx == -1) {
		unsigned int trytarget = rand() % (w * h);
		if (lash_mapSimpleLayerPeek(map.layer_path, trytarget) == 0)
			idx = (int)trytarget;
	}
	
	dumpmappathlayer(&map, 1, idx, -1, -1, NULL, 0);
	
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
	
	if (savedtarget == -1) {
		target = rand() % spacescount;
		printf("Target pointer pos is %d\n", (int)target);
		targetglobalindex = *(spaces + target);
	} else {
		printf("Target is global pointer pos %d (from saved file)\n", (int)savedtarget);
		targetglobalindex = savedtarget;
	}
		
	
	printf("Target is %d\n", (int)targetglobalindex);
	dumpmappathlayer(&map, 1, idx, targetglobalindex, -1, NULL, 0);

	// CALCULATE THE PATH
	
	lash_debugTimerReset(&timer);
	
	for (i = 0; i < testcount; i++) {
		long ticksoutputcompensate = 0;
		lash_pathSimpleInit(&path, &map, idx, targetglobalindex);
		
		lash_treeDumpInit(2);
		lash_treeDumpAdd(path.opentree, "open");
		lash_treeDumpAdd(path.closedtree, "closed");
		
		lash_pathSimpleNext(&path, &space);
		//pathopenstepsresult = lash_pathSimpleStepProcess(&path, &map, space, obstacles, obstaclestotal);
	
	
		attemptedsteps = 0;
		lash_debugTimerStart(&timer);
		while (space->index != path.target) {
			
			if (dumpeverystep && i == 0) {
				lash_debugTimerPause(&timer);
				printf("\ndumping idx %u\n", (unsigned int)space->index);
				dumpmappathlayer(&map, 1, idx, targetglobalindex, space->index, NULL, 0);
				lash_debugTimerPause(&timer);
			}
			
			pathopenstepsresult = lash_pathSimpleStepProcess(&path, &map, space, obstacles, obstaclestotal);
			attemptedsteps++;
			lash_pathSimpleNext(&path, &space);
			
			if (space == NULL) {
				impossible = 1;
				break;
			}
			
		} 
		lash_debugTimerStop(&timer);
	} //for test count
	
	
	printf("number of runs: %u\ntotal ticks: %li\naverage ticks: %f\nattempted steps: %u\n", timer.count, timer.total, lash_debugTimerGetAverage(&timer), attemptedsteps);
		
	i = 0;
	
	if (!impossible) {
		finalpath = (lash_path_simple_space_t*)malloc(sizeof(lash_path_simple_space_t) * (path.closedtree->count + 1));
		*(finalpath + 0) = *space;
		
		do {
			space = (finalpath + i)->parent;
			if (space != NULL) {
				i++;
				*(finalpath + i) = *space;
			}
		} while (space != NULL);
		
		dumpmappathlayer(&map, 1, idx, targetglobalindex, -1, finalpath, i);
		free(finalpath);
	} else {
		printf("No path possible!\n");
	}
	
	lash_pathSimpleFree(&path);
	lash_mapSimpleFree(&map);
	free(spaces);
	free(obstacles);
	
	return 0;
	
}

