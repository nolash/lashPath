#ifndef LASH_GAME_MAP_SIMPLE_H_
#define LASH_GAME_MAP_SIMPLE_H_

#define LASH_GAME_MAP_SIMPLE_DEFAULT_LAYERS 3

#include "liblashgame/lash_game_standard.h"

typedef unsigned short lash_map_simple_layer_item_t;

typedef struct lash_map_simple_layer_t lash_map_simple_layer_t;

struct lash_map_simple_layer_t {
	lash_map_simple_layer_item_t val;
};

typedef struct lash_map_simple_t lash_map_simple_t;

struct lash_map_simple_t {
	unsigned int *w;
	unsigned int *h;
	lash_map_simple_layer_t **layers;
	unsigned char layer_count;
	lash_map_simple_layer_t *layer_path;
};

lash_map_simple_t * lash_mapSimpleInit(lash_map_simple_t *map, unsigned int *w, unsigned int *h);
void lash_mapSimpleFree(lash_map_simple_t *map);
lash_map_simple_layer_t * lash_mapSimpleAddLayer(lash_map_simple_t *map);
void lash_mapSimpleLayerPoke(lash_map_simple_layer_t *layer, const lash_game_map_index_t index, const lash_map_simple_layer_item_t value);
lash_map_simple_layer_item_t lash_mapSimpleLayerPeek(lash_map_simple_layer_t *layer, const lash_game_map_index_t index);

#endif // LASH_GAME_MAP_SIMPLE_H_
