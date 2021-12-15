#ifndef _BLOCKLIST_H
#define _BLOCKLIST_H

#include "regions.h"

typedef struct BLOCK_NODE block_node;

struct BLOCK_NODE
{
	rsize_t size;
	void *block_start;
	block_node * next;
};

#define BLOCK_ALIGNMENT 8

void *new_block_list();
block_node *add_block(rsize_t block_size, void *list_top, rsize_t data_size, void *data_start);
block_node *find_block(void *block_start, void *list_top);
Boolean delete_block(block_node *target, void *list_top);
Boolean destroy_block_list(void *list_top);
block_node *first_block(void *list_top);

#endif
