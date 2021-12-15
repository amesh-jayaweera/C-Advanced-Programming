#ifndef _REGIONLIST_H
#define _REGIONLIST_H

#include "regions.h"

typedef struct REGION_NODE region_node;

struct REGION_NODE
{
	char *name;
	rsize_t size;
	rsize_t bytes_used;
	void *data;
	void *block_list;
	region_node *next;
};

region_node *insert();
Boolean delete_region(const char *target);
Boolean search_region(const char *target);
region_node *return_region(const char *target);
region_node *first_region();
region_node *next_region();

#endif
