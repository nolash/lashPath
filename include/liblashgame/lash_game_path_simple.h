#ifndef LASH_GAME_PATH_SIMPLE_H_
#define LASH_GAME_PATH_SIMPLE_H_

#include "liblashgame/lash_game_standard.h"
#include "liblashgame/lash_game_map.h"
#include "liblash/lash_tree3.h"

#define LASH_GAME_PATH_SIMPLE_SCORE_STRAIGHT 10
#define LASH_GAME_PATH_SIMPLE_SCORE_DIAGONAL 14

// Defines amount of resistance, not amount of movement
#define LASH_GAME_PATH_SIMPLE_OBSTACLE_MODIFIER_NONE 192
#define LASH_GAME_PATH_SIMPLE_OBSTACLE_MODIFIER_QUARTER 144
#define LASH_GAME_PATH_SIMPLE_OBSTACLE_MODIFIER_THIRD 128
#define LASH_GAME_PATH_SIMPLE_OBSTACLE_MODIFIER_HALF 96
#define LASH_GAME_PATH_SIMPLE_OBSTACLE_MODIFIER_TWOTHIRDS 64
#define LASH_GAME_PATH_SIMPLE_OBSTACLE_MODIFIER_THREEQUARTER 48
#define LASH_GAME_PATH_SIMPLE_OBSTACLE_MODIFIER_FULL 0

typedef struct lash_path_simple_obstacle_t lash_path_simple_obstacle_t;

struct lash_path_simple_obstacle_t {
	lash_map_simple_layer_item_t  val;
	unsigned char modifier;
};

typedef struct lash_path_simple_space_t lash_path_simple_space_t;

struct lash_path_simple_space_t {
	lash_tree_key_t f;
	unsigned char g;
	unsigned int h;
	lash_path_simple_space_t *parent;
	lash_game_map_index_t index;
};

typedef struct lash_path_simple_t lash_path_simple_t;

struct lash_path_simple_t {
	unsigned int *w;
	unsigned int *h;
	lash_tree_t *opentree;
	lash_tree_t *closedtree;
	lash_path_simple_space_t *spaces;
	unsigned int space_count;
	unsigned int space_capacity;
	lash_game_map_index_t source;
	lash_game_map_index_t target;
};

#ifdef __cplusplus 
extern "C" {
#endif

lash_path_simple_t * lash_pathSimpleInit(lash_path_simple_t *path, lash_map_simple_t *map, const lash_game_map_index_t offsetindex, const lash_game_map_index_t targetindex);
lash_path_simple_t * lash_pathSimpleUpdateSpace(lash_path_simple_t *path, lash_path_simple_space_t *currentspace, lash_path_simple_space_t *parent, const lash_game_map_index_t currentindex, const unsigned int g);
lash_path_simple_space_t * lash_pathSimpleAddSpace(lash_path_simple_t *path);
int lash_pathSimpleClose(lash_path_simple_t *path, lash_path_simple_space_t *space);
lash_path_simple_space_t * lash_pathSimpleNext(lash_path_simple_t *path, lash_path_simple_space_t **space);
float lash_pathSimpleCheckModifier(const lash_map_simple_layer_item_t *layeritem, const lash_path_simple_obstacle_t *obstacleslist, const unsigned char obstaclescount);
unsigned int lash_pathSimpleStepProcess(lash_path_simple_t *path, lash_map_simple_t *map, lash_path_simple_space_t *center, const lash_path_simple_obstacle_t *obstacleslist, const unsigned char obstaclescount);
void lash_pathSimpleFree(lash_path_simple_t *path);
lash_path_simple_space_t * lash_pathSimpleFindIndexInTree(lash_tree_t *tree, lash_game_map_index_t index);
//lash_path_simple_space_t * lash_pathSimpleFindIndexInSpaces(lash_path_simple_t *path, lash_game_map_index_t index);

#ifdef __cplusplus 
}
#endif

#endif //LASH_GAME_PATH_SIMPLE_H_
