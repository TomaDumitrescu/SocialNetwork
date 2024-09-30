#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "friends.h"
#include "users.h"

void add_friendship(char *friend1, char *friend2, list_graph_t *network)
{
	int id1 = get_user_id(friend1), id2 = get_user_id(friend2);

	lg_add_edge(network, id1, id2);
	lg_add_edge(network, id2, id1);

	printf("Added connection %s - %s\n", friend1, friend2);
}

void remove_friendship(char *friend1, char *friend2, list_graph_t *network)
{
	int id1 = get_user_id(friend1), id2 = get_user_id(friend2);

	lg_remove_edge(network, id1, id2);
	lg_remove_edge(network, id2, id1);

	printf("Removed connection %s - %s\n", friend1, friend2);
}

void calculate_distance(char *friend1, char *friend2, list_graph_t *network)
{
	int id1 = get_user_id(friend1), id2 = get_user_id(friend2);
	int *v = calloc(MAX_USERS, sizeof(int));
	if (!v) {
		printf("Calloc failed!\n");
		exit(1);
	}

	// distance with one BFS
	int d = distance(id1, id2, network, v);
	if (d > 0)
		printf("The distance between %s - %s is %d\n", friend1, friend2, d);
	else
		printf("There is no way to get from %s to %s\n", friend1, friend2);

	free(v);
}

void suggestions(char *user, list_graph_t *network) {
	int id = get_user_id(user);

	/*
	 *	Algorithm:
	 *  - Mark all user's friends with -1 (avoiding user's friends)
	 *  - Iterate over users friends and find other friends,
	 * marking them with 1 (avoiding duplicates)
	 *  - Add the found friends to a list if they are not marked
	 * with 1 or -1
	 */

	int *v = calloc(MAX_USERS, sizeof(int));
	if (!v) {
		printf("Calloc failed!");
		exit(1);
	}

	ll_node_t *node = network->neighbors[id]->head;
	while (node) {
		int neighbor = *(int *)node->data;
		v[neighbor] = -1;

		node = node->next;
	}

	int *suggestions = calloc(MAX_USERS, sizeof(int)), num = 0;
	if (!suggestions) {
		printf("Calloc failed!");
		exit(1);
	}

	node = network->neighbors[id]->head;
	while (node) {
		int neighbor = *(int *)node->data;

		ll_node_t *deep_search = network->neighbors[neighbor]->head;
		while (deep_search) {
			int suggestion = *(int *)deep_search->data;
			if (v[suggestion] != 1 && v[suggestion] != -1) {
				// O(1) list add_first
				suggestions[num++] = suggestion;
				v[suggestion] = 1;
			}

			// don't mark user's friends as visited
			if (v[suggestion] != -1)
				v[suggestion] = 1;

			deep_search = deep_search->next;
		}

		node = node->next;
	}

	free(v);

	qsort(suggestions, num, sizeof(int), compare);

	for (int i = 0; i < num; i++) {
		char *name = get_user_name(suggestions[i]);
		printf("%s\n", name);
	}

	if (num == 0)
		printf("There are no suggestions for %s", user);

	free(suggestions);
}

void handle_input_friends(char *input, list_graph_t *network)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd)
		return;

	if (!strcmp(cmd, "add"))
		add_friendship(strtok(cmd, NULL), strtok(cmd, NULL), network);
	else if (!strcmp(cmd, "remove"))
		remove_friendship(strtok(cmd, NULL), strtok(cmd, NULL), network);
	else if (!strcmp(cmd, "suggestions"))
		suggestions(strtok(cmd, NULL), network);
	else if (!strcmp(cmd, "distance"))
		calculate_distance(strtok(cmd, NULL), strtok(cmd, NULL), network);
	else if (!strcmp(cmd, "common"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "friends"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "popular"))
		(void)cmd;
		// TODO: Add function

	free(commands);
}
