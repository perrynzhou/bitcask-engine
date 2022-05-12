
/*************************************************************************
	> File Name: list.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Fri 06 May 2022 12:50:07 PM UTC
 ************************************************************************/

#ifndef _LIST_H
#define _LIST_H
#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr) - (size_t)(&((type *)0)->member)))

#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev, type, member)

#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, n, head)                   \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		 pos = n, n = pos->next)

typedef struct list_node
{
	struct list_node *prev, *next;
}list_node;
list_node *list_alloc();
void list_add(list_node *link, list_node *prev);
void list_add_tail(list_node *link, list_node *head);
void list_del(list_node *link);
int list_empty(const list_node *head);
void list_destroy(list_node *link);
#endif