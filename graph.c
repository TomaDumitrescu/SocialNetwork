#include "graph.h"

linked_list_t *ll_create(unsigned int data_size)
{
	linked_list_t* ll;

	ll = malloc(sizeof(*ll));

	ll->head = NULL;
	ll->data_size = data_size;
	ll->size = 0;

	return ll;
}

ll_node_t* get_nth_node(linked_list_t* list, unsigned int n)
{
	if (!list) {
		return NULL;
	}
	
	unsigned int len = list->size - 1;
	unsigned int i;
	ll_node_t* node = list->head;
	
	n = MIN(n, len);
	
	for (i = 0; i < n; ++i) {
		node = node->next;
	}
	
	return node;
}

void ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
	ll_node_t *prev, *curr;
	ll_node_t* new_node;
	
	if (!list) {
		return;
	}
	
	if (n > list->size) {
		n = list->size;
	}
	
	curr = list->head;
	prev = NULL;
	while (n > 0) {
		prev = curr;
		curr = curr->next;
		--n;
	}

	new_node = malloc(sizeof(*new_node));
	new_node->data = malloc(list->data_size);
	memcpy(new_node->data, new_data, list->data_size);

	new_node->next = curr;
	if (prev == NULL) {
		/* or n == 0. */
		list->head = new_node;
	} else {
		prev->next = new_node;
	}

	list->size++;
}

ll_node_t *ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
	ll_node_t *prev, *curr;
	
	if (!list || !list->head) {
		return NULL;
	}
	
	if (n > list->size - 1) {
		n = list->size - 1;
	}
	
	curr = list->head;
	prev = NULL;
	while (n > 0) {
		prev = curr;
		curr = curr->next;
		--n;
	}
	
	if (prev == NULL) {
		/* or n == 0. */
		list->head = curr->next;
	} else {
		prev->next = curr->next;
	}
	
	list->size--;
	
	return curr;
}

unsigned int ll_get_size(linked_list_t* list)
{
	if (!list) {
		return -1;
	}
	
	return list->size;
}

void ll_free(linked_list_t** pp_list)
{
	ll_node_t* currNode;
	
	if (!pp_list || !*pp_list) {
		return;
	}
	
	while (ll_get_size(*pp_list) > 0) {
		currNode = ll_remove_nth_node(*pp_list, 0);
		free(currNode->data);
		currNode->data = NULL;
		free(currNode);
		currNode = NULL;
	}
	
	free(*pp_list);
	*pp_list = NULL;
}

void ll_print_int(linked_list_t* list)
{
	ll_node_t* curr;
	
	if (!list) {
		return;
	}
	
	curr = list->head;
	while (curr != NULL) {
		printf("%d ", *((int*)curr->data));
		curr = curr->next;
	}
	
	printf("\n");
}

void ll_print_string(linked_list_t* list)
{
	ll_node_t* curr;
	
	if (!list) {
		return;
	}
	
	curr = list->head;
	while (curr != NULL) {
		printf("%s ", (char*)curr->data);
		curr = curr->next;
	}
	
	printf("\n");
}

list_graph_t* lg_create(int nodes)
{
	list_graph_t *g = malloc(sizeof(*g));
	g->nodes = nodes;
	g->neighbors = calloc(sizeof(linked_list_t *), nodes);
	for (int i = 0; i < nodes; i++)
		g->neighbors[i] = ll_create(sizeof(int));
	return g;
}

void lg_add_edge(list_graph_t* graph, int src, int dest)
{
	ll_add_nth_node(graph->neighbors[src], graph->neighbors[src]->size + 1, (const void *)&dest);
}

int lg_has_edge(list_graph_t* graph, int src, int dest)
{
	for (unsigned int i = 0; i < graph->neighbors[src]->size; i++) {
		ll_node_t *node = get_nth_node(graph->neighbors[src], i);
		int x = *(int *)node->data;
		if (x == dest)
			return 1;
	}
	return 0;
}

void lg_remove_edge(list_graph_t* graph, int src, int dest)
{
	for (unsigned int i = 0; i < graph->neighbors[src]->size; i++) {
		ll_node_t *node = get_nth_node(graph->neighbors[src], i);
		int x = *(int *)node->data;
		if (x == dest) {
			ll_node_t *rm = ll_remove_nth_node(graph->neighbors[src], i);
			free(rm->data);
			rm->data = NULL;
			free(rm);
			rm = NULL;
			return;
		}
	}
}

void lg_free(list_graph_t* graph)
{
	for (int j = 0; j < graph->nodes; j++) {
		ll_free(&graph->neighbors[j]);
		free(graph->neighbors[j]);
	}
	free(graph->neighbors);
	free(graph);
}

void lg_print_graph(list_graph_t* graph)
{
	for (int j = 0; j < graph->nodes; j++) {
		printf("%d: ", j);
		for (unsigned int i = 0; i < graph->neighbors[j]->size; i++) {
			ll_node_t *node = get_nth_node(graph->neighbors[j], i);
			int x = *(int *)node->data;
			printf("%d ", x);
		}
		printf("\n");
	}
}

queue_t*
q_create(unsigned int data_size, unsigned int max_size)
{
	queue_t* q = calloc(1, sizeof(*q));

	q->data_size = data_size;
	q->max_size = max_size;

	q->buff = malloc(max_size * sizeof(*q->buff));

	return q;
}

unsigned int
q_get_size(queue_t* q)
{
	return !q ? 0 : q->size;
}

unsigned int
q_is_empty(queue_t* q)
{
	return !q ? 1 : !q->size;
}

void*
q_front(queue_t* q)
{
	if (!q || !q->size)
		return NULL;
	return q->buff[q->read_idx];
}

bool
q_dequeue(queue_t* q)
{
	if (!q || !q->size)
		return false;

	free(q->buff[q->read_idx]);

	q->read_idx = (q->read_idx + 1) % q->max_size;
	--q->size;
	return true;
}

bool
q_enqueue(queue_t* q, void* new_data)
{
	void* data;
	if (!q || q->size == q->max_size)
		return false;

	data = malloc(q->data_size);
	memcpy(data, new_data, q->data_size);

	q->buff[q->write_idx] = data;
	q->write_idx = (q->write_idx + 1) % q->max_size;
	++q->size;

	return true;
}

void
q_clear(queue_t* q)
{
	unsigned int i;
	if (!q || !q->size)
		return;

	for (i = q->read_idx; i != q->write_idx; i = (i + 1) % q->max_size)
		free(q->buff[i]);

	q->read_idx = 0;
	q->write_idx = 0;
	q->size = 0;
}

void
q_free(queue_t* q)
{
	if (!q)
		return;

	q_clear(q);
	free(q->buff);
	free(q);
}

/*
*   v initialized with 1 and d with INF, except d[dest] = 0
*   Time complexity: O(num_nodes + num_edges)
*/
int distance(int src, int dest, list_graph_t* graph, int *v)
{
	int d_sol = -1, *d = calloc(MAX_USERS, sizeof(int));
	if (!d) {
		printf("Calloc failed!");
		exit(1);
	}

	for (int i = 0; i < MAX_USERS; i++)
		d[i] = INF;

	d[src] = 0;

	queue_t *q = q_create(sizeof(int), MAX_USERS);

	v[src] = 1;
	q_enqueue(q, (void *)&src);
	while (!q_is_empty(q)) {
		int node = *(int *)q_front(q);
		q_dequeue(q);

		ll_node_t *search = graph->neighbors[node]->head;
		while (search) {
			int neighbor = *(int *)search->data;

			if (!v[neighbor]) {
				v[neighbor] = 1;
				q_enqueue(q, (void *)&neighbor);
			}

			d[neighbor] = MIN(d[neighbor], 1 + d[node]);

			search = search->next;
		}
	}

	q_free(q);

	if (d[dest] != INF)
		d_sol = d[dest];

	free(d);

	return d_sol;
}

int compare(const void *obj, const void *oth) {
	return (*(int*)obj - *(int*)oth);
}
