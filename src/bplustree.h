#ifndef _BPLUS_TREE_H
#define _BPLUS_TREE_H

#include "list.h"
#include "utils.h"
#define MIN_CACHE_NUM 5

typedef int key_t;

typedef struct bplus_node
{
        off_t self;
        off_t parent;
        off_t prev;
        off_t next;
        int type;
        /* If leaf node, it specifies  count of entries,
         * if non-leaf node, it specifies count of children(branches) */
        int children;
} bplus_node;

typedef struct free_block
{
        struct list_head link;
        off_t offset;
} free_block;

struct bplus_tree
{
        char *caches;
        int used[MIN_CACHE_NUM];
        char filename[1024];
        int fd;
        int level;
        off_t root;
        off_t file_size;
        struct list_head free_blocks;
};

void bplus_tree_dump(struct bplus_tree *tree);
long bplus_tree_get(struct bplus_tree *tree, key_t key);
int bplus_tree_put(struct bplus_tree *tree, key_t key, long data);
long bplus_tree_get_range(struct bplus_tree *tree, key_t key1, key_t key2);
struct bplus_tree *bplus_tree_init(char *filename, int block_size);
void bplus_tree_deinit(struct bplus_tree *tree);
int bplus_open(char *filename);
void bplus_close(int fd);

#endif
