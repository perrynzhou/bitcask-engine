#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bplustree.h"
static void _proc(bplus_tree *tree, char op, int n)
{
        switch (op) {
                case 'i':
                        bplus_tree_put(tree, n, n);
                        break;
                case 'r':
                        bplus_tree_put(tree, n, 0);
                        break;
                case 's':
                        printf("key:%d data_index:%ld\n", n, bplus_tree_get(tree, n));
                        break;
                default:
                        break;
        }       
}

static int number_process(bplus_tree *tree, char op)
{
        int c, n = 0;
        int start = 0, end = 0;

        while ((c = getchar()) != EOF) {
                if (c == ' ' || c == '\t' || c == '\n') {
                        if (start != 0) {
                                if (n >= 0) {
                                        end = n;
                                } else {
                                        n = 0;
                                }
                        }

                        if (start != 0 && end != 0) {
                                if (start <= end) {
                                        for (n = start; n <= end; n++) {
                                                _proc(tree, op, n);
                                        }
                                } else {
                                        for (n = start; n >= end; n--) {
                                                _proc(tree, op, n);
                                        }
                                }
                        } else {
                                if (n != 0) {
                                        _proc(tree, op, n);
                                }
                        }

                        n = 0;
                        start = 0;
                        end = 0;

                        if (c == '\n') {
                                return 0;
                        } else {
                                continue;
                        }
                }

                if (c >= '0' && c <= '9') {
                        n = n * 10 + c - '0';
                } else if (c == '-' && n != 0) {
                        start = n;
                        n = 0;
                } else {
                        n = 0;
                        start = 0;
                        end = 0;
                        while ((c = getchar()) != ' ' && c != '\t' && c != '\n') {
                                continue;
                        }
                        ungetc(c, stdin);
                }
        }

        printf("\n");
        return -1;
}

static void command_tips(void)
{
        printf("i: Insert key. e.g. i 1 4-7 9\n");
        printf("r: Remove key. e.g. r 1-100\n");
        printf("s: Search by key. e.g. s 41-60\n");
        printf("d: Dump the tree structure.\n");
        printf("q: quit.\n");
}

static void command_process(bplus_tree *tree)
{
        int c;
        printf("Please input command (Type 'h' for help): ");
        for (; ;) {
                switch (c = getchar()) {
                case EOF:
                        printf("\n");
                case 'q':
                        return;
                case 'h':
                        command_tips();
                        break;
                case 'd':
                        bplus_tree_dump(tree);
                        break;
                case 'i':
                case 'r':
                case 's':
                        if (number_process(tree, c) < 0) {
                                return;
                        }
                case '\n':
                        printf("Please input command (Type 'h' for help): ");
                default:
                        break;
                }
        }
}

int main(void)
{
        bplus_tree *tree = NULL;
        while (tree == NULL) {
                int fd =open("/tmp/btree", O_RDWR | O_CREAT);
                tree = bplus_tree_init("/tmp/btree",fd,4096);
        }
        command_process(tree);
        bplus_tree_dump(tree);
        bplus_tree_deinit(tree);

        return 0;
}
