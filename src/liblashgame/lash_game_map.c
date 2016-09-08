#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "liblashgame/lash_game_map.h"
#include "liblashgame/lash_game_standard.h"

lash_map_simple_t * lash_mapSimpleInit(lash_map_simple_t *map, unsigned int *w, unsigned int *h) {
	map->w = (unsigned int*)malloc(sizeof(unsigned int));
	map->h = (unsigned int*)malloc(sizeof(unsigned int));
	map->layer_count = 0;
	*(map->w) = *w;
	*(map->h) = *h;
	map->layers = (lash_map_simple_layer_t**)malloc(sizeof(lash_map_simple_layer_t*) * LASH_GAME_MAP_SIMPLE_DEFAULT_LAYERS);
	map->layer_path = lash_mapSimpleAddLayer(map);
	return map;
}

void lash_mapSimpleFree(lash_map_simple_t *map) {
	int i;
	
	if (map->w != NULL)
		free(map->w);
	
	if (map->h != NULL)
		free(map->h);
	
	for (i = 0; i < map->layer_count; i++) {
		if ((map->layers + i) != NULL)
			free(map->layers + i);
	}
	
}

lash_map_simple_layer_t * lash_mapSimpleAddLayer(lash_map_simple_t *map) {
	lash_map_simple_layer_t *newlayer;
	newlayer = (lash_map_simple_layer_t*)malloc(*(map->w) * *(map->h) * sizeof(lash_map_simple_layer_item_t));
	if (newlayer == NULL)
		return NULL;
	*(map->layers + map->layer_count) = newlayer;
	map->layer_count ++;
	return newlayer;
}

void lash_mapSimpleLayerPoke(lash_map_simple_layer_t *layer, const lash_game_map_index_t index, const lash_map_simple_layer_item_t value) {
	if (layer == NULL)
		return;
	
	(layer+index)->val = value;
}

lash_map_simple_layer_item_t lash_mapSimpleLayerPeek(lash_map_simple_layer_t *layer, const lash_game_map_index_t index) {
	if (layer == NULL)
		return -1;
	
	return (layer+index)->val;
}

lash_map_simple_t * lash_mapSimpleClip(lash_map_simple_t *clipmap, lash_map_simple_t *sourcemap, const unsigned int clip_x_radius, const unsigned int clip_y_radius, const lash_game_map_index_t clip_origin_index, enum lash_game_map_alignment clip_origin_alignment, int include_path_only) {

	int i;
	
	unsigned int delta_left;
	unsigned int delta_right;
	unsigned int delta_top;
	unsigned int delta_bottom;
	
	unsigned int clip_y_top;
	unsigned int clip_y_bottom;
	unsigned int clip_x_left;
	unsigned int clip_x_right;
	
	unsigned int clip_width;
	unsigned int clip_height;
	
	//lash_game_coords_cartesian_t map_coords_bounds;
	lash_game_map_index_t map_index_last;
	
	// if no valid source map
	if (sourcemap == NULL)
		return NULL;
	else if (*sourcemap->w == 0 || *sourcemap->h == 0)
		return NULL;
		
	map_index_last = (*sourcemap->w * *sourcemap->h) - 1;
	
	if (clip_origin_index > map_index_last)
		return NULL;
	
	// this will always clip from center
	// also make possible clip from topleft
	delta_top = (int)floor(clip_origin_index / *sourcemap->w);
	delta_bottom = *sourcemap->h - delta_top - 1;
	delta_left = clip_origin_index - delta_top * *sourcemap->w;
	delta_right = ((delta_top + 1) * *sourcemap->w) - clip_origin_index - 1;
	
	clip_y_top = clip_y_radius > delta_top ? delta_top : clip_y_radius;
	clip_y_bottom = clip_y_radius > delta_bottom ? delta_bottom : clip_y_radius;
	clip_x_left = clip_x_radius > delta_left ? delta_left : clip_x_radius;
	clip_x_right = clip_x_radius > delta_right ? delta_right : clip_x_radius;
	
	clip_width = (clip_x_right + clip_x_left + 1);
	clip_height = (clip_y_bottom + clip_y_top + 1);
	
	// we now have the size of the clip and the coords to cut
	
	lash_mapSimpleInit(clipmap, &clip_width, &clip_height);
	
	if (!include_path_only) {
		for (i = 0; i < sourcemap->layer_count; i++) {
			lash_mapSimpleAddLayer(clipmap);
		}
	}
	
	// and fill in with values from the main map
	
	for (i = clip_height - 1; i > -1; i--) {	
		int cpos = clip_origin_index - (*sourcemap->w * (i - delta_bottom)) - clip_x_left;
		memcpy((*clipmap->layers + ((clip_height - 1 - i) * clip_width)), (*sourcemap->layers + cpos), clip_width * sizeof(lash_map_simple_layer_item_t));
	}	
	
	return clipmap;
}
