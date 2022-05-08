
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

typedef struct list_head
{
	struct list_head *prev, *next;
}list_head;
void list_init(list_head *link);
void list_add(list_head *link, list_head *prev);
void list_add_tail(list_head *link, list_head *head);
void list_del(list_head *link);
int list_empty(const list_head *head);
#endif