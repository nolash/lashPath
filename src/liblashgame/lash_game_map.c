#include <stdlib.h>
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
	free(map->w);
	free(map->h);
	free(map->layers);
	free(map->layer_path);
}

lash_map_simple_layer_t * lash_mapSimpleAddLayer(lash_map_simple_t *map) {
	lash_map_simple_layer_t *newlayer;
	newlayer = (lash_map_simple_layer_t*)calloc(*(map->w) * *(map->h), sizeof(lash_map_simple_layer_t));
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
