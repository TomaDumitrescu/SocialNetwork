#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REALLOC_REF 25

typedef struct node_t node_t;
struct node_t {
	void *data;
	node_t **children;
	int size;
	int num_reallocs;
};

typedef struct tree_t tree_t;
struct tree_t {
	node_t *root;
};

#ifndef MAX_USERS
#define MAX_USERS 1000
#endif

#endif