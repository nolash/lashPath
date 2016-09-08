// check modifier check

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "liblashgame/lash_game_path_simple.h"
#include "liblashgame/lash_game_map.h"

int main() {
	clock_t test_clock_total = 0.f;
	clock_t begin;
	unsigned int w = 29;
	unsigned int h = 21;
	lash_path_simple_obstacle_t *o = (lash_path_simple_obstacle_t*)malloc(sizeof(lash_path_simple_obstacle_t)*3);
	o->val = 1;
	o->modifier = LASH_GAME_PATH_SIMPLE_OBSTACLE_MODIFIER_FULL;
	(o+1)->val = 2;
	(o+1)->modifier = LASH_GAME_PATH_SIMPLE_OBSTACLE_MODIFIER_HALF;
	
	lash_map_simple_t map;
	lash_mapSimpleInit(&map, &w, &h);
	lash_mapSimpleLayerPoke(map.layer_path, 10, 1);
	lash_mapSimpleLayerPoke(map.layer_path, 20, 2);
	lash_mapSimpleLayerPoke(map.layer_path, 30, 3);
	
	float r;
	
	begin = clock();
	r = lash_pathSimpleCheckModifier(&(map.layer_path + 10)->val, o, 2);
	test_clock_total += clock() - begin;
	printf("Check idx 10 content 1: %f\n", r);
	
	begin = clock();
	r = lash_pathSimpleCheckModifier(&(map.layer_path + 20)->val, o, 2);
	test_clock_total += clock() - begin;
	printf("Check idx 20 content 2: %f\n", r);
	
	begin = clock();
	r = lash_pathSimpleCheckModifier(&(map.layer_path + 30)->val, o, 2);
	test_clock_total += clock() - begin;
	printf("Check idx 30 content 3: %f\n", r);
	
	test_clock_total /= 3;
	
	printf("Total checktime (avg of 3): %lu ticks @ %li pr sec = %.7f secs\n\n", (long)test_clock_total, CLOCKS_PER_SEC, ((double)test_clock_total / CLOCKS_PER_SEC));
	
	return 0;
}
