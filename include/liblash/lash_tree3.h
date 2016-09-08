#ifndef LASH_TREE_H_
#define LASH_TREE_H_

typedef long int lash_tree_key_t;

typedef struct lash_tree_t {
	lash_tree_key_t **key;
	void **item;
	//unsigned int shiftpos;
	unsigned int count;
	unsigned int capacity;
	//unsigned int foldbackthreshold;
	int average;
} lash_tree_t;

#ifdef __cplusplus 
extern "C" {
#endif

lash_tree_t * lash_treeInit(lash_tree_t *tree, const unsigned int size) ;
int lash_treePush(lash_tree_t *tree, void *item, lash_tree_key_t *key);
int lash_treeShift(lash_tree_t *tree, void **item);
int lash_treeRemove(lash_tree_t *tree, unsigned int position, void **item);
int _lash_treeProcess(lash_tree_t *tree, unsigned int currentposition, int direction);
void lash_treeFree(lash_tree_t *tree);
#ifdef __cplusplus 
}
#endif


#endif // LASH_TREE_H_
 
/**
 * \file lash_tree3.h
 * \brief Implementation of binary tree for sorting generic objects
 * 
 * \author Louis Holbrook
 * \version 0.2
 * 
 * \todo Possibility to remove from other positions in the tree than start 
 *  
 * This library aims to be a fast and small way of storing generic structs (or even just integers) in a binary tree for fast retrieval in sorted order.
 * It was written as a backend for A* pathfinding code
 * 
 * Structs are stored as *void* pointers, which means they can reference anything at all.
 * Every "entry" in the tree will have one *void* pointer and one pointer to the sort value. At least one member of the struct must be of `lash_tree_key_t` type, which this pointer will point to.
 * Theoretically, different struct types can be stored in the different entries of the tree.
 * 
 */
/**
 * \var typedef lash_tree_key_t 
 * \param lash_tree_key_t Sort value type for `lash_tree_t` 
 */
 
/**
 * \var typedef lash_tree_t
 * \param **key Pointers to the sort key values
 * \param **item Pointers to the sorted data structures
 * \param count Current amount of items in the tree
 * \param capacity Maximum amount of items allocated
 * \param average Sort value at the middle of the array (currently unused)
 */
 
/**
 * \struct lash_tree_t
 * \param **key Pointers to the sort key values
 * \param **item Pointers to the sorted data structures
 * \param count Current amount of items in the tree
 * \param capacity Maximum amount of items allocated
 * \param average Sort value at the middle of the array (currently unused)
 */

 
/**
 * \fn lash_treeInit(lash_tree_t *tree, const unsigned int size)
 * \brief Allocates memory and initializes members
 * 
 * \param *tree Pointer to allocate memory to
 * \param size Amount of items to allocate
 * \return NULL if not successful
 * 
 */

/**
 * \fn lash_treeFree(lash_tree_t *tree)
 * \brief Frees up memory
 * 
 * \param *tree Tree struct to free
 * 
 */

/**
 * \fn lash_treePush(lash_tree_t *tree, void *item, lash_tree_key_t *key)
 * \brief Add a new item to the tree
 * 
 * \param *tree Tree struct to add to
 * \param *item Data structure to add
 * \param *key Pointer to the sort key value in `*item`
 * \return Number of items in the tree after adding. If tree is full -1 is returned.
 * 
 * The data structure must have one member of type `lash_tree_key_t`
 * if `*key` is omitted, it will be assumed that the first position at the address of `*item` is of this type, and will be used as the sort value for this item.
 * The item is added to the end of the tree, and stepwise moved up the tree until it meets a lower value.
 * 
 */

/**
 * \fn lash_treeShift(lash_tree_t *tree, void **item)
 * \brief Retrieves and removes the first item from the tree
 * 
 * \param *tree Tree struct to shift from
 * \param **item Pointer to pass the retrieved data structure pointer in
 * \return Wrapper for `lash_treeRemove` with position argument 0
 * 
 * \sa lash_treeRemove
 * 
 */

/**
 * \fn lash_treeRemove(lash_tree_t *tree, unsigned int position, void **item)
 * \brief Retrieves and removes an item from the tree
 * 
 * \param *tree Tree struct to shift from
 * \param position What sort position to remove from.
 * \param **item Pointer to pass the retrieved data structure pointer in
 * \return Number of items in the tree after removal. If the tree was already empty -1 is returned.
 * 
 * \sa lash_treePush
 * 
 * \warning Currently only position 0 is tested. Setting another value causes undefined behavior and will probably not work.
 * 
 * After the item is retrieved, the item at the end of the tree is moved up to replace it, and stepwise moved down the tree until meets a higher value.
 * 
 * 
 */
