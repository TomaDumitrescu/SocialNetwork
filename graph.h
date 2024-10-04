#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 1000

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX_STRING_SIZE	256

typedef struct ll_node_t
{
	void* data;
	struct ll_node_t* next;
} ll_node_t;

typedef struct linked_list_t
{
	ll_node_t* head;
	unsigned int data_size;
	unsigned int size;
} linked_list_t;

linked_list_t *ll_create(unsigned int data_size);
ll_node_t* get_nth_node(linked_list_t* list, unsigned int n);
void ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data);
ll_node_t *ll_remove_nth_node(linked_list_t* list, unsigned int n);
unsigned int ll_get_size(linked_list_t* list);
void ll_free(linked_list_t** pp_list);
void ll_print_int(linked_list_t* list);
void ll_print_string(linked_list_t* list);

typedef struct
{
	linked_list_t** neighbors;
	int nodes;
} list_graph_t;

list_graph_t* lg_create(int nodes);
void lg_add_edge(list_graph_t* graph, int src, int dest);
int lg_has_edge(list_graph_t* graph, int src, int dest);
void lg_remove_edge(list_graph_t* graph, int src, int dest);
void lg_free(list_graph_t* graph);
void lg_print_graph(list_graph_t* graph);

#include <stdbool.h>

typedef struct queue_t queue_t;
struct queue_t
{
unsigned int max_size;
unsigned int size;
unsigned int data_size;
unsigned int read_idx;
unsigned int write_idx;
void** buff;
};

queue_t*
q_create(unsigned int data_size, unsigned int max_size);
unsigned int
q_get_size(queue_t* q);
unsigned int
q_is_empty(queue_t* q);
void*
q_front(queue_t* q);
bool
q_dequeue(queue_t* q);
bool
q_enqueue(queue_t* q, void* new_data);
void
q_clear(queue_t* q);
void
q_free(queue_t* q);

#define INF 100000000

int distance(int src, int dest, list_graph_t* graph, int *v);
int compare(const void *obj, const void *oth);

#endif
