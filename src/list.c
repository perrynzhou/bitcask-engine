/*************************************************************************
  > File Name: list.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 06 May 2022 12:50:11 PM UTC
 ************************************************************************/

#include "list.h"
#include <stdio.h>

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
        prev->next = next;
        next->prev = prev;
}

static inline void __list_add(struct list_head *link, struct list_head *prev, struct list_head *next)
{
        link->next = next;
        link->prev = prev;
        next->prev = link;
        prev->next = link;
}

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
        prev->next = next;
        next->prev = prev;
}


void list_init(struct list_head *link)
{
        link->prev = link;
        link->next = link;
}


void list_add(struct list_head *link, struct list_head *prev)
{
        __list_add(link, prev, prev->next);
}

void list_add_tail(struct list_head *link, struct list_head *head)
{
	__list_add(link, head->prev, head);
}

void list_del(struct list_head *link)
{
        __list_del(link->prev, link->next);
        list_init(link);
}

int list_empty(const struct list_head *head)
{
	return head->next == head;
}