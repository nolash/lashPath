#ifndef LASH_GAME_MAP_SIMPLE_H_
#define LASH_GAME_MAP_SIMPLE_H_

#define LASH_GAME_MAP_SIMPLE_DEFAULT_LAYERS 3

#include "liblashgame/lash_game_standard.h"

	// considered doing this with pointer to pointer, to allow one map object be a clip of another and point to the same data sources, but:
	//
	// upside: data only in one place; change it in map, change it in clip
	// downsize: pointers are twice the size of unsigned int, twice the memory to reserve for maps and then later move for clips
	// Here is a possible implementation:
	//
	// keep the actual values in a separate array,
	// (this is the same solution as in lash_game_path_simple)
	//
	// c.layervalues = (unsigned int*)malloc(sizeof(unsigned int) * 400);
	// *c.layervalues = 10;
	// *c.layervalues = 20;
	//
	// Highest pointer level is pointers to the individual layers
	//
	// c.layers = (unsigned int ***)malloc(sizeof(unsigned int) * 2);
	//
	// Second pointer level the individual entries are pointers to the values in c.layervalues
	//
	// *(c.layers + 0) = (unsigned int **)malloc(sizeof(unsigned int) * 100);
	// *(c.layers + 1) = (unsigned int **)malloc(sizeof(unsigned int) * 100);
	//
	// Pointers can be assigned on the third level
	//
	// **(c.layers + 0) = c.layervalues;
	// *(*(c.layers + 1) + 1) = c.layervalues;
	//
	// printf("pointer address layer %p : %p layer %p : %p in layervalues %p\n", *(c.layers + 0), **(c.layers + 0), *(c.layers + 1), *(*(c.layers + 1) + 1), c.layervalues); 
	
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

enum lash_game_map_alignment {
	LASH_GAME_MAP_OFFSET_CENTER,
	LASH_GAME_MAP_OFFSET_TOPLEFT,
	LASH_GAME_MAP_OFFSET_BOTTOMLEFT,
	LASH_GAME_MAP_OFFSET_BOTTOMRIGHT,
	LASH_GAME_MAP_OFFSETP_TOPRIGHT
};

lash_map_simple_t * lash_mapSimpleInit(lash_map_simple_t *map, unsigned int *w, unsigned int *h);
void lash_mapSimpleFree(lash_map_simple_t *map);
lash_map_simple_layer_t * lash_mapSimpleAddLayer(lash_map_simple_t *map);
void lash_mapSimpleLayerPoke(lash_map_simple_layer_t *layer, const lash_game_map_index_t index, const lash_map_simple_layer_item_t value);
lash_map_simple_layer_item_t lash_mapSimpleLayerPeek(lash_map_simple_layer_t *layer, const lash_game_map_index_t index);
lash_map_simple_t * lash_mapSimpleClip(lash_map_simple_t *clipmap, lash_map_simple_t *sourcemap, const unsigned int clip_x_radius, const unsigned int clip_y_radius, const lash_game_map_index_t clip_origin_index, enum lash_game_map_alignment clip_origin_alignment, int include_path_only);

#endif // LASH_GAME_MAP_SIMPLE_H_



/**
 * \fn  lash_mapSimpleClip(lash_map_simple_t *clipmap, lash_map_simple_t *sourcemap, const unsigned int clip_x_radius, const unsigned int clip_y_radius, const lash_game_map_index_t clip_origin_index, enum lash_game_map_clip_alignment clip_origin_alignment, int include_path_only)
 * 
 * \todo Support for defining other alignments from index (especially topleft). Currently it only calculates clip with index in center and in "radius" rectangle around.
 * \todo Support for copying all layers, now it only copies the first (layer_path)
 * \todo Support for copying (back) from clip to map
*/
