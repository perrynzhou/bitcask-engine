#ifndef _BPLUS_TREE_H
#define _BPLUS_TREE_H

#include "list.h"
#include "utils.h"
#define MIN_CACHE_NUM 5

typedef int key_t;

typedef struct bplus_tree
{
        char *caches;
        int used[MIN_CACHE_NUM];
        char *name;
        int fd;
        int level;
        off_t root;
        off_t file_size;
        struct list_head free_blocks;
        key_compare cmp;
}bplus_tree;


void bplus_tree_dump(bplus_tree *tree);
long bplus_tree_get(bplus_tree *tree, key_t key);
int bplus_tree_put(bplus_tree *tree, key_t key, long data);
long bplus_tree_get_range(bplus_tree *tree, key_t key1, key_t key2);
bplus_tree *bplus_tree_init(char *name,int fd, int block_size);
void bplus_tree_deinit(bplus_tree *tree);

#endif
