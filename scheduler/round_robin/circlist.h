#include <linux/list.h>
#include <linux/ratelimit.h>

#define	circ_entry(ptr, type, member) container_of(ptr, type, member)

//GERI
struct circular_lista{
	struct list_head* head;
	struct list_head* last_chosen;
	unsigned int count;
};
typedef struct circular_lista circular_list;

void circularlist_insert(circular_list * list, struct list_head * after_node, struct list_head * new_node);
void circularlist_remove(circular_list * list, struct list_head * node);
void circularlist_traverse_debug(circular_list * list);
