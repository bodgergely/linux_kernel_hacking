#ifndef CIRCLIST_H
#define CIRCLIST_H
#include "circlist.h"
#endif

void circularlist_insert(circular_list * list, struct list_head * after_node, struct list_head * new_node)
{
    if (new_node) {
        if (list->count > 0) {
            new_node->next = after_node;
            new_node->prev = after_node->prev;
            new_node->prev->next = new_node;
            after_node->prev = new_node;
        }
        else {
            /* Adding the first element */
            list->head = new_node;
            list->head->next = list->head;
            list->head->prev = list->head;
        }
        list->count++;
    }
}

void circularlist_remove(circular_list * list, struct list_head * node)
{
	int numSeenHead;
	struct list_head* link = list->head;
	if(!link){
		//printk_ratelimited(KERN_ERR "list->head is NULL!");
		return;
	}

	numSeenHead = 0;
	while(link!=node){
		if(link == list->head){
			numSeenHead++;
			if(numSeenHead > 1){
				//printk_ratelimited(KERN_ERR "Seen head %d times! Probably bug", numSeenHead);
			}
		}
		link = link->next;
	}

	if(list->count > 1){
		link->prev->next = link->next;
		link->next->prev = link->prev;
		if(link == list->head)
			list->head = link->next;
	}
	else{
		list->head = NULL;
	}

	list->count--;


}

void circularlist_traverse_debug(circular_list * list)
{

	int i = 0;
	struct list_head* link = list->head;
	if(!link){
		//printk_ratelimited(KERN_ERR "list->head is NULL!\n");
		return;
	}

	do
	{
		//printk_ratelimited(KERN_ERR "node %d at %p\n", i++, link);
		link = link->next;
	}while(link!=list->head);

}

