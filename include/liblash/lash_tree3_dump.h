#ifndef LASH_TREE_DUMP_H_
#define LASH_TREE_DUMP_H_

#include "liblash/lash_tree3.h"

struct lash_tree_dump_monitor {
	lash_tree_t **tree;
	char **name;
} lash_tree_dump_monitor;

#ifdef __cplusplus 
extern "C" {
#endif
void lash_treeDump(lash_tree_t *tree, char *comment);
int lash_treeDumpInit(unsigned int count);
int lash_treeDumpAdd(lash_tree_t *tree, char *name);
void lash_treeDumpFree();

#ifdef __cplusplus 
}
#endif

#endif // LASH_TREE_DUMP_H_

/**
 * \file lash_tree3_dump.h 
 * \brief Dump contents of `lash_tree3` objects
 * 
 * \author Louis Holbrook 
 * \version 0.2
 * 
 */
 
/**
 * 
 * \struct lash_tree_dump_monitor
 * \brief Container pointing to trees elegible for dumping
 * \var **tree Pointers to trees
 * \var **name Name of tree to display when dumping
 * 
 */

/**
 * \fn lash_treeDump(lash_tree_t *tree, char *comment)
 * \brief Dumps content of tree in sort order to **STDOUT**
 * 
 * \param *tree Tree to dump.
 * \param *comment Optional text string to display in front of dump
 * 
 * If `*tree` is specified, only this tree will be dumped.
 * Note that trees have to be added to `lash_tree_dump_monitor` by `lash_treeDumpAdd` or they will be skipped.
 * 
 * \todo Parametric output formatting
 */

/**
 * \fn lash_treeDumpInit(unsigned int count)
 * \brief Memory allocation
 * 
 * \param count number of trees to allocate
 * \return 1 if allocation failed, 0 if no error.
 */

/**
 * \fn lash_treeDumpAdd(lash_tree_t *tree, char *name)
 * \brief Add tree to the list of trees for dumping
 * 
 * \param *tree Tree to add
 * \param *name Name to display in front of output
 * \return 1 if capacity is exceeded, 0 otherwise
 * 
 */
 
/**
 * \fn lash_treeDumpFree()
 * \brief Free memory from `lash_tree_dump_monitor`
 * 
 * \param *tree Tree to free
 */
