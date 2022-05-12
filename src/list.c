/*************************************************************************
  > File Name: list.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Fri 06 May 2022 12:50:11 PM UTC
 ************************************************************************/

#include "list.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
static inline void __list_del(list_node *prev, list_node *next)
{
        prev->next = next;
        next->prev = prev;
}

static inline void __list_add(list_node *link, list_node *prev, list_node *next)
{
        link->next = next;
        link->prev = prev;
        next->prev = link;
        prev->next = link;
}

list_node *list_alloc()
{
        list_node *link = (list_node *)calloc(1, sizeof(list_node));
        assert(link != NULL);
        link->prev = NULL;
        link->next = NULL;
        return link;
}

void list_add(list_node *link, list_node *prev)
{
        __list_add(link, prev, prev->next);
}

void list_add_tail(list_node *link, list_node *head)
{
        __list_add(link, head->prev, head);
}

void list_destroy(list_node *link) {
        if(link) {
                free(link);
                link = NULL;
        }
}
void list_del(list_node *link)
{
        __list_del(link->prev, link->next);
        list_destroy(link);
}

int list_empty(const list_node *head)
{
        return head->next == head;
}