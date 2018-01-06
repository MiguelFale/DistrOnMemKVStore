#ifndef _LIST_PRIVATE_H
#define _LIST_PRIVATE_H

#include "list.h"

struct list_t {
	int tamanho;
	struct node_t *cabeca;
	struct node_t *cauda;
};

struct node_t {
	struct entry_t *client;
	struct node_t *next;
	struct node_t *prev;
};

#endif
